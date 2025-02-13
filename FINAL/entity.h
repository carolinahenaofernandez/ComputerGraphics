#pragma once

#include "framework.h"
#include "mesh.h"
#include "image.h"

enum class eRenderMode {
    POINTCLOUD,
    WIREFRAME,
    TRIANGLES,
    TRIANGLES_INTERPOLATED
};

class Entity {
public:
    Mesh* mesh;
    Matrix44 model;
    Color color;
    int id;
    bool is_moving = true; // Por defecto, la entidad se mueve
    bool useZBuffer = true;
    void ToggleMovement();
    Image* texture;
    Image* normalMap;


    eRenderMode mode;
    

    Entity();
    explicit Entity(Mesh* m);
    Entity(Mesh* m, const Matrix44& mat);
    virtual ~Entity();

    virtual void Update(float seconds_elapsed);
    virtual void RenderLab2(Image* framebuffer, Camera* camera, const Color& c);
    void RenderLab3(Image* framebuffer, Camera* camera, FloatImage* zBuffer);

};
