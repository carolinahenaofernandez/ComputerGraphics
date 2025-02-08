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

void Entity::Update(float seconds_elapsed) {
    // Placeholder for now
}

void Entity::Render(Image* framebuffer, Camera* camera, const Color& c) {
    // Placeholder for rendering logic
    if (!mesh || !camera) return;
    std::cout << "Rendering entity..." << std::endl;
    const std::vector<Vector3>& vertices = mesh->GetVertices(); //Create a pointer to use it in the for loop
    for(size_t i = 0; i < vertices.size(); i+=3){ //The += 3 is to process each triangle separately
        Vector3 worldVertices[3]; //Store world space coordinates
        Vector3 projectedVertices[3]; //Store clip space coordinates
        bool validTriangle = true;
        
        for(int j = 0; j < 3; ++j){
            worldVertices[j] = model * vertices[i + j];
            bool negZ = false;
            projectedVertices[j] = camera->ProjectVector(worldVertices[j], negZ);
            if(projectedVertices[j].x < -1 || projectedVertices[j].x > 1 ||
               projectedVertices[j].y < -1 || projectedVertices[j].y > 1 ||
               projectedVertices[j].z < -1 || projectedVertices[j].z > 1){
                validTriangle = false;
            }
        }
        if(!validTriangle) continue;
        for(int j = 0; j < 3; ++j){
            projectedVertices[j].x = (projectedVertices[j].x + 1.0f) * 0.5f * framebuffer->width;
            projectedVertices[j].y = (1.0f - projectedVertices[j].y) * 0.5f * framebuffer->height; //flip y-axis.
        }

        //Hola pookie, acabo de editar aixo, crec que se veuen millor els triangles aixi.
        framebuffer->DrawLineDDA(projectedVertices[0].x, projectedVertices[0].y, projectedVertices[2].x, projectedVertices[2].y, c);
        framebuffer->DrawLineDDA(projectedVertices[2].x, projectedVertices[2].y, projectedVertices[1].x, projectedVertices[1].y, c);
        framebuffer->DrawLineDDA(projectedVertices[1].x, projectedVertices[1].y, projectedVertices[0].x, projectedVertices[0].y, c);
        
        
    }
}
