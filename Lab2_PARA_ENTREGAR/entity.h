#pragma once

#include "framework.h"
#include "mesh.h"
#include "image.h"


class Entity {
public:
    Mesh* mesh;  // Pointer to a mesh object (not owned by Entity)
    Matrix44 model;  // Transformation matrix

    // Default Constructor
    Entity();

    // Constructor with mesh
    explicit Entity(Mesh* m);
    
    // Constructor with mesh and model matrix
    Entity(Mesh* m, const Matrix44& mat);

    // Virtual destructor to allow subclassing
    virtual ~Entity();

    // Update function for animation
    virtual void Update(float seconds_elapsed);

    // Render function
    virtual void Render(Image* framebuffer, Camera* camera, const Color& c);
};
