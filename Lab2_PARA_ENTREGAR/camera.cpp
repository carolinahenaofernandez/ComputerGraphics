void Camera::UpdateViewMatrix()
{
    // Reset Matrix (Identity)
    view_matrix.SetIdentity();

    // Comment this line to create your own projection matrix!
    //SetExampleViewMatrix();

    // Remember how to fill a Matrix4x4 (check framework slides)
    // Careful with the order of matrix multiplications, and be sure to use normalized vectors!
    Vector3 front = (center - eye).Normalize();
    Vector3 right = up.Cross(front).Normalize();
    Vector3 new_up = front.Cross(right).Normalize();
    
    // Create the view matrix rotation
    // ...
    // view_matrix.M[3][3] = 1.0;
    view_matrix.M[0][0] = right.x;
    view_matrix.M[0][1] = right.y;
    view_matrix.M[0][2] = right.z;
    view_matrix.M[1][0] = new_up.x;
    view_matrix.M[1][1] = new_up.y;
    view_matrix.M[1][2] = new_up.z;
    view_matrix.M[2][0] = -front.x;
    view_matrix.M[2][1] = -front.y;
    view_matrix.M[2][2] = -front.z;

    // Translate view matrix
    // ...
    view_matrix.TranslateLocal(-eye.x, -eye.y, -eye.z);

    UpdateViewProjectionMatrix();
}

// Create a projection matrix
void Camera::UpdateProjectionMatrix()
{
    // Reset Matrix (Identity)
    projection_matrix.SetIdentity();

    // Comment this line to create your own projection matrix!
    //SetExampleProjectionMatrix();

    // Remember how to fill a Matrix4x4 (check framework slides)
    
    if (type == PERSPECTIVE) {
        // projection_matrix.M[2][3] = -1;
        // ...
        projection_matrix.M[0][0] = (1.0f / tan(fov * 0.5f * DEG2RAD)) / aspect;
        projection_matrix.M[1][1] = 1.0f / tan(fov * 0.5f * DEG2RAD);
        projection_matrix.M[2][2] = -(far_plane + near_plane) / (far_plane - near_plane);
        projection_matrix.M[2][3] = -1;
        projection_matrix.M[3][2] = -2 * far_plane * near_plane / (far_plane - near_plane);
        projection_matrix.M[3][3] = 0;
    }
    else if (type == ORTHOGRAPHIC) {
        // ...
        projection_matrix.M[0][0] = 2.0f / (right - left);
        projection_matrix.M[1][1] = 2.0f / (top - bottom);
        projection_matrix.M[2][2] = -2.0f / (far_plane - near_plane);
        projection_matrix.M[3][0] = -(right + left) / (right - left);
        projection_matrix.M[3][1] = -(top + bottom) / (top - bottom);
        projection_matrix.M[3][2] = -(far_plane + near_plane) / (far_plane - near_plane);
        projection_matrix.M[3][3] = 1;
    }

    UpdateViewProjectionMatrix();
}
