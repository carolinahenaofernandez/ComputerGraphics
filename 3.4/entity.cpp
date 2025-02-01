//
//  entity.cpp
//  ComputerGraphics
//
//  Created by Carolina on 29/1/25.
//

#include "entity.h"
#include "camera.h"
#include "utils.h"

#include <cmath>  // For sin and cos functions

template <typename T>
T clamp(T value, T min, T max) {
    return (value < min) ? min : (value > max) ? max : value;
}

// Default constructor: no mesh, identity model matrix
Entity::Entity() {
    mesh = nullptr;
    model.SetIdentity();  // Set the transformation matrix to identity
}

// Constructor with mesh
Entity::Entity(Mesh* m) {
    mesh = m;
    model.SetIdentity();
}

// Constructor with mesh and model matrix
Entity::Entity(Mesh* m, const Matrix44& mat) {
    mesh = m;
    model = mat;
}

// Destructor
Entity::~Entity() {
    // No need to delete the mesh here because it's handled externally
}

void Entity::Render(Image* framebuffer, Camera* camera, const Color& c){
    if (!mesh) return;  // Ensure the entity has a mesh

    std::cout << "Rendering entity..." << std::endl;

    // Iterate through mesh vertices
    for (size_t i = 0; i < mesh->GetVertices().size(); i += 3) {
        Vector3 worldVertices[3];

        // 1. Transform local vertices to world space
        for (int j = 0; j < 3; ++j) {
            worldVertices[j] = model * mesh->GetVertices()[i + j];
            std::cout << "World Vertex [" << j << "]: "
                      << worldVertices[j].x << ", "
                      << worldVertices[j].y << ", "
                      << worldVertices[j].z << std::endl;
        }

        // 2. Project vertices from world space to clip space
        Vector3 clipVertices[3];
        for (int j = 0; j < 3; ++j) {
            bool negZ = false;
            clipVertices[j] = camera->ProjectVector(worldVertices[j], negZ);
            std::cout << "Clip Vertex [" << j << "]: "
                      << clipVertices[j].x << ", "
                      << clipVertices[j].y << ", "
                      << clipVertices[j].z << std::endl;
        }

        // 3. Only render if within the clip space [-1, 1]^3
        bool insideClip = true;
        for (int j = 0; j < 3; ++j) {
            if (clipVertices[j].x < -1 || clipVertices[j].x > 1 ||
                clipVertices[j].y < -1 || clipVertices[j].y > 1 ||
                clipVertices[j].z < -1 || clipVertices[j].z > 1) {  // Change this line
                insideClip = false;
                break;
            }
        }

        if (!insideClip) {
            std::cout << "Triangle is outside clip space, skipping." << std::endl;
            continue;
        }

        // 4. Convert clip space to screen space
        Vector2 screenVertices[3];
        for (int j = 0; j < 3; ++j) {
            screenVertices[j].x = clamp(((clipVertices[j].x + 1) * 0.5f) * framebuffer->width, 0.0f, (float)framebuffer->width - 1);
            screenVertices[j].y = clamp(((1 - (clipVertices[j].y + 1) * 0.5f) * framebuffer->height), 0.0f, (float)framebuffer->height - 1);

            std::cout << "Screen Vertex [" << j << "]: "
                      << screenVertices[j].x << ", "
                      << screenVertices[j].y << std::endl;
        }

        // 5. Draw triangle edges using DDA algorithm
        std::cout << "Drawing triangle edges..." << std::endl;
        DrawLineDDA(framebuffer, screenVertices[0], screenVertices[1], c);
        DrawLineDDA(framebuffer, screenVertices[1], screenVertices[2], c);
        DrawLineDDA(framebuffer, screenVertices[2], screenVertices[0], c);
    }
}


void Entity::Update(float seconds_elapsed) {
    static float time_accumulator = 0.0f;
    time_accumulator += seconds_elapsed;

    if (!mesh) return; // Verifica que la entidad tenga una malla válida

    model.SetIdentity(); // Reinicia la matriz de transformación

    if (mesh->name == "Entity1") {
        // 🔄 Rotación en el eje Y
        model.Translate(-2.5f, 1.0f, 0);
        model.Rotate(time_accumulator * 1.5f, Vector3(0, 1, 0)); // Rotación más rápida
        model.Rotate(180 * DEG2RAD, Vector3(1, 0, 0)); // Corrige inversión
    }
    else if (mesh->name == "Entity2") {
        // 🔀 Traslación oscilante en el eje X
        float movement = sin(time_accumulator * 2.0f) * 1.0f; // Movimiento más dinámico
        model.Translate(movement, 1.0f, 0);
        model.Rotate(180 * DEG2RAD, Vector3(1, 0, 0)); // Corrige inversión
    }
    else if (mesh->name == "Entity3") {
        // 📏 Escalado dinámico con oscilación
        float scale = 1.0f + 0.3f * sin(time_accumulator * 2.0f);
        model.Translate(2.5f, 1.0f, 0);
        model.Scale(scale, scale, scale);
        model.Rotate(180 * DEG2RAD, Vector3(1, 0, 0)); // Corrige inversión
    }
}
