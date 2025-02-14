#pragma once

#include "framework.h"
#include "mesh.h"
#include "image.h"

// Enum class to define different rendering modes for an entity
enum class eRenderMode {
    POINTCLOUD,          // Render the entity as a point cloud (just points)
    WIREFRAME,           // Render the entity as a wireframe (edges of the mesh)
    TRIANGLES,           // Render the entity as solid triangles (filled triangles)
    TRIANGLES_INTERPOLATED // Render the entity with interpolated colors and textures
};

class Entity {
public:

    Mesh* mesh;
    Matrix44 model;
    Color color;
    Image* texture;
    Image* normalMap;

    eRenderMode mode= eRenderMode::TRIANGLES_INTERPOLATED;         // Rendering mode of the entity (determines how the entity is rendered)

    // Unique identifier for the entity
    int id;
    // Unique identifier for the entity
    bool useZBuffer = true;
    // Flag to determine whether the entity is moving or stationary (defaults to moving)
    bool is_moving = true;

    // Function to toggle the movement state of the entity
    void ToggleMovement();

    Entity();
    explicit Entity(Mesh* m);
    Entity(Mesh* m, const Matrix44& mat);
    virtual ~Entity();
    virtual void Update(float seconds_elapsed);

    // Render method for Lab2 (rendering using basic color)
    virtual void RenderLab2(Image* framebuffer, Camera* camera, const Color& c);
    // Render method for Lab3 (rendering using Z-buffer and texture)
    void RenderLab3(Image* framebuffer, Camera* camera, FloatImage* zBuffer);
};
