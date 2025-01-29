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

        // Iterate through mesh vertices
        for (size_t i = 0; i < mesh->GetVertices().size(); i += 3) {
            Vector3 worldVertices[3];

            // 1. Transform local vertices to world space
            for (int j = 0; j < 3; ++j) {
                worldVertices[j] = model * mesh->GetVertices()[i + j];
            }

            // 2. Project vertices from world space to clip space
            Vector3 clipVertices[3];
            for (int j = 0; j < 3; ++j) {
                bool negZ = false;
                clipVertices[j] = camera->ProjectVector(worldVertices[j], negZ);
            }

            // 3. Only render if within the clip space [-1, 1]^3
            bool insideClip = true;
            for (int j = 0; j < 3; ++j) {
                if (clipVertices[j].x < -1 || clipVertices[j].x > 1 ||
                    clipVertices[j].y < -1 || clipVertices[j].y > 1 ||
                    clipVertices[j].z < -1 || clipVertices[j].z > 1) {
                    insideClip = false;
                    break;
                }
            }
            if (!insideClip) continue;

            // 4. Convert clip space to screen space
            Vector2 screenVertices[3];
            for (int j = 0; j < 3; ++j) {
                screenVertices[j].x = (clipVertices[j].x + 1) * 0.5 * framebuffer->width;
                screenVertices[j].y = (1 - (clipVertices[j].y + 1) * 0.5) * framebuffer->height;
            }

            // 5. Draw triangle edges using DDA algorithm
            DrawLineDDA(framebuffer, screenVertices[0], screenVertices[1], c);
            DrawLineDDA(framebuffer, screenVertices[1], screenVertices[2], c);
            DrawLineDDA(framebuffer, screenVertices[2], screenVertices[0], c);
        }

}

void Entity::Update(float seconds_elapsed) {
    static float time_accumulator = 0.0f;
    time_accumulator += seconds_elapsed;

    if (!mesh) return; // Ensure mesh is valid

    if (mesh->name == "Entity1") {
        model.SetIdentity();
        model.Rotate(time_accumulator, Vector3(0, 1, 0));
    }
    else if (mesh->name == "Entity2") {
        model.SetIdentity();
        float movement = sin(time_accumulator) * 2.0f;
        model.Translate(movement, 0, 0);
    }
    else if (mesh->name == "Entity3") {
        model.SetIdentity();
        float scale = 1.0f + 0.5f * sin(time_accumulator);
        model.Scale(scale, scale, scale);  // FIXED
    }
}

