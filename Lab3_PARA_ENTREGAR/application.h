/*
    + This class encapsulates the application, is in charge of creating the data, getting the user input, process the update and render.
*/

#pragma once

#include "main/includes.h"
#include "framework.h"
#include "image.h"
#include "camera.h"
#include "entity.h"

class Application
{
public:

    // Window
    SDL_Window* window = nullptr;
    int window_width;
    int window_height;

    // Time variable used for time-based calculations
    float time;
    // List of entities in the scene
    std::vector<Entity*> entities;
    // Pointer to the camera used for rendering the scene
    Camera* camera;
    // Currently selected property for the camera (near, far, or FOV)
    float current_property;
    // Indicates which property is currently being modified ('N' = near, 'F' = far, 'V' = FOV)
    char property_mode;
    // Last mouse position to calculate mouse movement
    Vector2 last_mouse_position;
    // Index for the current scene being rendered (1 for a single entity, 2 for multiple animated entities)
    int current_scene=1;
    // Depth buffer to store depth values for each pixel (used for occlusion)
    FloatImage zBuffer;
    // Texture for normal mapping
    Image* texture_normal;
    // Texture for color and specular effects
    Image* texture_color_specular;
    // Flag to indicate whether the application is in Lab3 mode (to handle different behaviors)
    bool isLab3;


    // Input
    const Uint8* keystate;
    int mouse_state; // Tells which buttons are pressed
    Vector2 mouse_position; // Last mouse position
    Vector2 mouse_delta; // Mouse movement in the last frame

    void OnKeyPressed(SDL_KeyboardEvent event);
    void OnMouseButtonDown(SDL_MouseButtonEvent event);
    void OnMouseButtonUp(SDL_MouseButtonEvent event);
    void OnMouseMove(SDL_MouseButtonEvent event);
    void OnWheel(SDL_MouseWheelEvent event);
    void OnFileChanged(const char* filename);

    // CPU Global framebuffer
    Image framebuffer;

    // Constructor and main methods
    Application(const char* caption, int width, int height);
    ~Application();

    void Init(void);
    void Render(void);
    void Update(float dt);

    // Other methods to control the app
    void SetWindowSize(int width, int height) {
        glViewport(0, 0, width, height);
        this->window_width = width;
        this->window_height = height;
        this->framebuffer.Resize(width, height);
    }

    Vector2 GetWindowSize()
    {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        return Vector2(float(w), float(h));
    }
};
