#include "camera.h"

#include "main/includes.h"
#include <iostream>


Camera::Camera()
{
	view_matrix.SetIdentity();
	SetOrthographic(-1,1,1,-1,-1,1);
}

Vector3 Camera::GetLocalVector(const Vector3& v)
{
	Matrix44 iV = view_matrix;
	if (iV.Inverse() == false)
		std::cout << "Matrix Inverse error" << std::endl;
	Vector3 result = iV.RotateVector(v);
	return result;
}

Vector3 Camera::ProjectVector(Vector3 pos, bool& negZ)
{
	Vector4 pos4 = Vector4(pos.x, pos.y, pos.z, 1.0);
	Vector4 result = viewprojection_matrix * pos4;
	negZ = result.z < 0;
	if (type == ORTHOGRAPHIC)
		return result.GetVector3();
	else
		return result.GetVector3() / result.w;
}

void Camera::Rotate(float angle, const Vector3& axis)
{
	Matrix44 R;
	R.SetRotation(angle, axis);
	Vector3 new_front = R * (center - eye);
	center = eye + new_front;
	UpdateViewMatrix();
}

void Camera::Move(Vector3 delta)
{
	Vector3 localDelta = GetLocalVector(delta);
	eye = eye - localDelta;
	center = center - localDelta;
	UpdateViewMatrix();
}

void Camera::SetOrthographic(float left, float right, float top, float bottom, float near_plane, float far_plane)
{
	type = ORTHOGRAPHIC;

	this->left = left;
	this->right = right;
	this->top = top;
	this->bottom = bottom;
	this->near_plane = near_plane;
	this->far_plane = far_plane;

	UpdateProjectionMatrix();
}

void Camera::SetPerspective(float fov, float aspect, float near_plane, float far_plane)
{
	type = PERSPECTIVE;

	this->fov = fov;
	this->aspect = aspect;
	this->near_plane = near_plane;
	this->far_plane = far_plane;

	UpdateProjectionMatrix();
}

void Camera::LookAt(const Vector3& eye, const Vector3& center, const Vector3& up)
{
	this->eye = eye;
	this->center = center;
	this->up = up;

	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix() {
    // Reset Matrix (Identity)
    view_matrix.SetIdentity();

    // Comment this line to create your own projection matrix!
    // SetExampleViewMatrix();

    // Compute forward vector (normalized direction from eye to center)
    Vector3 forward = (center - eye).Normalize();

    // Compute right vector (perpendicular to up & forward)
    Vector3 right = up.Cross(forward).Normalize();

    // Recompute true up vector (perpendicular to right & forward)
    Vector3 realUp = forward.Cross(right).Normalize();

    // Fill the view matrix using basis vectors
    view_matrix.m[0] = right.x;  view_matrix.m[4] = right.y;  view_matrix.m[8]  = right.z;  view_matrix.m[12] = -right.Dot(eye);
    view_matrix.m[1] = realUp.x; view_matrix.m[5] = realUp.y; view_matrix.m[9]  = realUp.z; view_matrix.m[13] = -realUp.Dot(eye);
    view_matrix.m[2] = -forward.x; view_matrix.m[6] = -forward.y; view_matrix.m[10] = -forward.z; view_matrix.m[14] = forward.Dot(eye);
    view_matrix.m[3] = 0.0f; view_matrix.m[7] = 0.0f; view_matrix.m[11] = 0.0f; view_matrix.m[15] = 1.0f;

    // Apply translation (TranslateLocal moves everything opposite to camera movement)
    view_matrix.TranslateLocal(-eye.x, -eye.y, -eye.z);

    // Update the combined view-projection matrix
    UpdateViewProjectionMatrix();
}

// Create a projection matrix
void Camera::UpdateProjectionMatrix() {
    // Reset Matrix (Identity)
    projection_matrix.SetIdentity();

    // Comment this line to create your own projection matrix!
    // SetExampleProjectionMatrix();

    if (type == PERSPECTIVE) {
        // Convert FOV from degrees to radians
        float fovRadians = fov * (M_PI / 180.0f);
        float tanHalfFov = tan(fovRadians / 2.0f);
        float range = near_plane - far_plane;

        // Perspective projection matrix
        projection_matrix.m[0] = 1.0f / (aspect * tanHalfFov);
        projection_matrix.m[5] = 1.0f / tanHalfFov;
        projection_matrix.m[10] = (far_plane + near_plane) / range;
        projection_matrix.m[11] = -1.0f;
        projection_matrix.m[14] = (2.0f * far_plane * near_plane) / range;
        projection_matrix.m[15] = 0.0f;
    }
    else if (type == ORTHOGRAPHIC) {
        // Orthographic projection matrix
        projection_matrix.m[0] = 2.0f / (right - left);
        projection_matrix.m[5] = 2.0f / (top - bottom);
        projection_matrix.m[10] = -2.0f / (far_plane - near_plane);
        projection_matrix.m[12] = -(right + left) / (right - left);
        projection_matrix.m[13] = -(top + bottom) / (top - bottom);
        projection_matrix.m[14] = -(far_plane + near_plane) / (far_plane - near_plane);
        projection_matrix.m[15] = 1.0f;
    }

    // Update the combined view-projection matrix
    UpdateViewProjectionMatrix();
}

void Camera::UpdateViewProjectionMatrix()
{
	viewprojection_matrix = projection_matrix * view_matrix;
}

Matrix44 Camera::GetViewProjectionMatrix() 
{
	UpdateViewMatrix();
	UpdateProjectionMatrix();

	return viewprojection_matrix;
}

// The following methods have been created for testing.
// Do not modify them.

void Camera::SetExampleViewMatrix()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);
	glGetFloatv(GL_MODELVIEW_MATRIX, view_matrix.m );
}

void Camera::SetExampleProjectionMatrix()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (type == PERSPECTIVE)
		gluPerspective(fov, aspect, near_plane, far_plane);
	else
		glOrtho(left,right,bottom,top,near_plane,far_plane);

	glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix.m );
	glMatrixMode(GL_MODELVIEW);
}
