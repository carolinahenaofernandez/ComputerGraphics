//
//  entity.h
//  ComputerGraphics
//
//  Created by Carolina on 29/1/25.
//

#pragma once

#include "framework.h"
#include "mesh.h"
#include "image.h"


class Entity {
public:
    Mesh* mesh;  // Pointer to a mesh object
    Matrix44 model;  // Transformation matrix

    // Default Constructor
    Entity();

    // Constructor with mesh
    Entity(Mesh* m);
    
    // Update function for animation
    void Update(float seconds_elapsed);

    // Constructor with mesh and model matrix
    Entity(Mesh* m, const Matrix44& mat);

    // Destructor
    ~Entity();
    
    void Render(Image* framebuffer, Camera* camera, const Color& c);
};
