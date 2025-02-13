#include "entity.h"
#include "camera.h"
#include "utils.h"
#include <cmath>
#include "image.h"

Entity::Entity() {
    mesh = nullptr;
    model.SetIdentity();
}

Entity::Entity(Mesh* m) {
    mesh = m;
    model.SetIdentity();
}

Entity::Entity(Mesh* m, const Matrix44& mat) {
    mesh = m;
    model = mat;
}


Entity::~Entity() {}


void Entity::Update(float seconds_elapsed) {
    if (!is_moving) return; // Si está desactivado, no hacer nada
    
    static float time_acumulator = 0.0f;
    time_acumulator += seconds_elapsed;

    if (id == 1) {
        model.SetIdentity();
        model.Translate(-1.5f, sin(time_acumulator) * 0.5f, 0.0f);
    } else if (id == 2) {
        model.SetIdentity();
        model.Translate(1.5f, 0.0f, 0.0f);
        model.RotateLocal(time_acumulator, Vector3(0,1,0));
    } else if (id == 3) {
        model.SetIdentity();
        float scale = 1.0f + sin(time_acumulator) * 0.5f;
        model.M[0][0] = scale;
        model.M[1][1] = scale;
        model.M[2][2] = scale;
    }
    model.Translate(1.0f * seconds_elapsed, 0.0f, 0.0f);  // Ejemplo de movimiento
    model.RotateLocal(1.0f * seconds_elapsed, Vector3(0, 1, 0));  // Ejemplo de rotación
    
}

void Entity::ToggleMovement() {
    is_moving = !is_moving; // Toggle the movement state
}

void Entity::RenderLab2(Image* framebuffer, Camera* camera, const Color& c) {
    if (!mesh || !camera) return;
    
    const std::vector<Vector3>& vertices = mesh->GetVertices();
    for (size_t i = 0; i < vertices.size(); i += 3) {
        Vector3 worldVertices[3];
        Vector3 projectedVertices[3];
        bool validTriangle = true;
        
        for (int j = 0; j < 3; ++j) {
            worldVertices[j] = model * vertices[i + j];
            bool negZ = false;
            projectedVertices[j] = camera->ProjectVector(worldVertices[j], negZ);
            if (projectedVertices[j].x < -1 || projectedVertices[j].x > 1 ||
                projectedVertices[j].y < -1 || projectedVertices[j].y > 1 ||
                projectedVertices[j].z < -1 || projectedVertices[j].z > 1) {
                validTriangle = false;
            }
        }
        if (!validTriangle) continue;
        
        for (int j = 0; j < 3; ++j) {
            projectedVertices[j].x = (projectedVertices[j].x + 1.0f) * 0.5f * framebuffer->width;
            projectedVertices[j].y = (1.0f - projectedVertices[j].y) * 0.5f * framebuffer->height;
        }
        
        framebuffer->DrawLineDDA(projectedVertices[0].x, projectedVertices[0].y, projectedVertices[2].x, projectedVertices[2].y, c);
        framebuffer->DrawLineDDA(projectedVertices[2].x, projectedVertices[2].y, projectedVertices[1].x, projectedVertices[1].y, c);
        framebuffer->DrawLineDDA(projectedVertices[1].x, projectedVertices[1].y, projectedVertices[0].x, projectedVertices[0].y, c);
        
    }
}

void Entity::RenderLab3(Image* framebuffer, Camera* camera, FloatImage* zBuffer) {
    if (!mesh || !camera || !zBuffer) return;

    const std::vector<Vector3>& vertices = mesh->GetVertices();
    const std::vector<Vector2>& uvs = mesh->GetUVs();
    
    for (size_t i = 0; i < vertices.size(); i += 3) {
        sTriangleInfo triangle;

        // Transform vertices from model space to world space
        Vector3 worldVertices[3];
        for (int j = 0; j < 3; ++j) {
            worldVertices[j] = model * vertices[i + j];
        }

        // Project from world space to screen space
        Vector3 screenVertices[3];
        bool negZ;
        for (int j = 0; j < 3; ++j) {
            Vector3 projected = camera->ProjectVector(worldVertices[j], negZ);
            screenVertices[j].x = (projected.x + 1.0f) * 0.5f * framebuffer->width;
            screenVertices[j].y = (1.0f - projected.y) * 0.5f * framebuffer->height;
            screenVertices[j].z = projected.z;
        }

        // Handle different rendering modes
        switch (mode) {
            case eRenderMode::POINTCLOUD:
                // Render only points (no edges or filled triangles)
                for (int j = 0; j < 3; ++j) {
                    framebuffer->SetPixel(screenVertices[j].x, screenVertices[j].y, Color(255, 255, 255));
                }
                break;

            case eRenderMode::WIREFRAME:
                // Draw only the wireframe of the triangle
                framebuffer->DrawLineDDA(screenVertices[0].x, screenVertices[0].y, screenVertices[1].x, screenVertices[1].y, Color(255, 255, 255));
                framebuffer->DrawLineDDA(screenVertices[1].x, screenVertices[1].y, screenVertices[2].x, screenVertices[2].y, Color(255, 255, 255));
                framebuffer->DrawLineDDA(screenVertices[2].x, screenVertices[2].y, screenVertices[0].x, screenVertices[0].y, Color(255, 255, 255));
                break;

            case eRenderMode::TRIANGLES:
                // Render solid triangles using white color (no texture)
                framebuffer->DrawTriangle(screenVertices[0].GetVector2(), screenVertices[1].GetVector2(), screenVertices[2].GetVector2(),
                                          Color(255, 255, 255), true, Color(255, 255, 255));
                break;

            case eRenderMode::TRIANGLES_INTERPOLATED:
                // Fill sTriangleInfo structure for interpolated rendering
                triangle.p0 = screenVertices[0];
                triangle.p1 = screenVertices[1];
                triangle.p2 = screenVertices[2];

                triangle.uv0 = uvs[i];
                triangle.uv1 = uvs[i + 1];
                triangle.uv2 = uvs[i + 2];

                // Here you can choose whether to use vertex colors or texture
                triangle.c0 = Color(255, 0, 0);  // Red
                triangle.c1 = Color(0, 255, 0);  // Green
                triangle.c2 = Color(0, 0, 255);  // Blue

                triangle.texture = (texture != nullptr) ? texture : nullptr;  // If texture is disabled, use colors

                // Call rasterization
                framebuffer->DrawTriangleInterpolated(triangle, zBuffer, useZBuffer);
                break;
        }
    }
}
