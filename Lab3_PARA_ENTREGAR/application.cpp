#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h"
#include "entity.h"
#include "camera.h"

Application::Application(const char* caption, int width, int height)
{
    this->window = createWindow(caption, width, height);

    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    this->mouse_state = 0;
    this->time = 0.f;
    this->window_width = w;
    this->window_height = h;
    this->keystate = SDL_GetKeyboardState(nullptr);

    this->framebuffer.Resize(w, h);
}

Application::~Application()
{
}

void Application::Init(void)
{
    std::cout << "Initiating app..." << std::endl;

    // Initialize the camera: Set position, target, and up vector for camera view
    camera = new Camera();
    camera->LookAt(Vector3(0, 0, 3), Vector3(0, 0.25, 0), Vector3(0, -1, 0));  // Look at a point in the scene
    camera->SetPerspective(45.0f, (float)framebuffer.width / (float)framebuffer.height, 0.1f, 100.0f);  // Set perspective projection

    // Load the mesh (3D model)
    Mesh* lee = new Mesh();
    if (!lee->LoadOBJ("meshes/lee.obj")) {
        std::cerr << "[ERROR] Failed to load lee.obj!\n";
        return;  // Exit if mesh loading fails
    }
    else {
        std::cout << "[INFO] Mesh lee.obj loaded successfully!\n";
    }

    // Load textures for the model (color specular and normal map)
    texture_color_specular = new Image();
    if (!texture_color_specular->LoadTGA("textures/lee_color_specular.tga", true)) {
        std::cerr << "[ERROR] Failed to load lee_color_specular.tga!\n";
        return;
    }

    texture_normal = new Image();
    if (!texture_normal->LoadTGA("textures/lee_normal.tga", true)) {
        std::cerr << "[ERROR] Failed to load lee_normal.tga!\n";
        return;
    }

    // Initialize Z-buffer (depth buffer) for occlusion testing
    zBuffer.Resize(framebuffer.width, framebuffer.height);

    // Create and configure entities (3 copies of the loaded mesh)
    for (int i = 0; i < 3; i++) {
        Entity* entity = new Entity();
        entity->mesh = lee;
        entity->texture = texture_color_specular;
        entity->normalMap = texture_normal;
        entity->id = i + 1;

        // Set different positions for the entities in the scene
        if (i == 0) entity->model.Translate(-1.5f, 0.0f, 0.0f);
        if (i == 1) entity->model.Translate(1.5f, 0.0f, 0.0f);
        if (i == 2) entity->model.Translate(0.0f, 0.0f, 0.0f);

        // Add the entity to the list of entities
        entities.push_back(entity);
        std::cout << "[INFO] Created Entity " << entity->id << " with Mesh and Textures.\n";
    }

    std::cout << "Lee model and textures loaded successfully!\n";
}

void Application::Render(void)
{
    std::cout << "Rendering frame..." << std::endl;  // Debug message

    // Clear the framebuffer (reset the scene to black)
    framebuffer.Fill(Color(0, 0, 0));

    // Initialize Z-buffer (if not done already) for occlusion testing
    FloatImage zBuffer(framebuffer.width, framebuffer.height);
    for (int y = 0; y < framebuffer.height; ++y) {
        for (int x = 0; x < framebuffer.width; ++x) {
            zBuffer.SetPixel(x, y, 1.0f);  // Set all depth values to 1 (farthest)
        }
    }

    // Check which scene to render:
    // Scene 1: Render a single entity, Scene 2: Render multiple animated entities
    if (current_scene == 1) {  // Render a single entity
        if (!entities.empty() && entities[2]) {  // Ensure there's at least one entity
            Entity* entity = entities[2];  // Select the entity with id=3
            Color entityColor = Color(255, 255, 255);  // Default color

            // Assign different colors based on the entity ID
            if (entity->id == 1) {
                entityColor = Color(0, 255, 0);  // Green
            }
            else if (entity->id == 2) {
                entityColor = Color(0, 0, 255);  // Blue
            }

            // Render the entity using the selected method
            if (isLab3) {  // If Lab3 is selected, use Z-buffer for rendering
                entity->RenderLab3(&framebuffer, camera, &zBuffer);
            }
            else {  // If not in Lab3, render using simple color
                entity->RenderLab2(&framebuffer, camera, entityColor);
            }

            std::cout << "Rendering single entity ID " << entity->id << " with color ("
                << entityColor.r << ", " << entityColor.g << ", " << entityColor.b << ")" << std::endl;
        }
    }
    else if (current_scene == 2) {  // Render multiple animated entities
        for (Entity* entity : entities) {
            if (entity) {
                Color entityColor = Color(255, 255, 255);  // Default color

                // Assign different colors based on the entity ID
                if (entity->id == 1) {
                    entityColor = Color(0, 255, 0);  // Green
                }
                else if (entity->id == 2) {
                    entityColor = Color(0, 0, 255);  // Blue
                }

                // Use the same logic for Z-buffer rendering
				if (isLab3) { //if isLab3 is equal to false then the 3 colored entities will be rendered in wireframe mode
                    entity->RenderLab3(&framebuffer, camera, &zBuffer);
                }
                else {
                    entity->RenderLab2(&framebuffer, camera, entityColor);
                }

                std::cout << "Rendering entity ID " << entity->id << " with color ("
                    << entityColor.r << ", " << entityColor.g << ", " << entityColor.b << ")" << std::endl;
            }
        }
    }

    // Final step: Render the framebuffer to the screen
    framebuffer.Render();
}


void Application::Update(float seconds_elapsed)
{
    // Update all entities in the scene
    for (Entity* entity : entities) {
        if (entity) {
            entity->Update(seconds_elapsed);  // Update each entity's state (e.g., position, animation)
        }
    }
}

void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
    bool updated = false;

    switch (event.keysym.sym) {
    case SDLK_ESCAPE:
        exit(0);  // Exit the application if ESC is pressed
        break;

        // Toggle between MESH TEXTURE and COLOR PER VERTEX
    case SDLK_t:
        for (auto& entity : entities) {
            // Toggle texture on/off for each entity
            entity->texture = (entity->texture == nullptr) ? texture_color_specular : nullptr;
        }
        std::cout << "[INFO] Toggled between mesh texture and vertex color.\n";
        break;

        // Toggle between OCCLUSIONS (Z-Buffer) and NO OCCLUSIONS
    case SDLK_z:
        for (auto& entity : entities) {
            entity->useZBuffer = !entity->useZBuffer;  // Toggle Z-buffer usage
        }
        std::cout << "[INFO] Toggled occlusions (Z-Buffer).\n";
        break;

        // Toggle between INTERPOLATED UVs and PLAIN COLOR
    case SDLK_c:
        for (auto& entity : entities) {
            if (entity->mode == eRenderMode::TRIANGLES) {
                entity->mode = eRenderMode::TRIANGLES_INTERPOLATED;  // Switch to interpolated mode
            }
            else if (entity->mode == eRenderMode::TRIANGLES_INTERPOLATED) {
                entity->mode = eRenderMode::TRIANGLES;  // Switch back to plain triangles
            }
        }
        std::cout << "[INFO] Toggled between interpolated UVs and plain color.\n";
        break;

        // Additional functionality: Switch between static and animated entities (toggle movement)
    /*case SDLK_s:
        for (auto& entity : entities) {
            entity->is_moving = !entity->is_moving;  // Toggle movement of entities
        }
        std::cout << "[INFO] Toggled between static and animated entities.\n";
        break;

        // Switch between different render modes for the entities
    case SDLK_3:
        for (auto& entity : entities) {
            entity->mode = eRenderMode::POINTCLOUD;  // Set render mode to POINTCLOUD
        }
        std::cout << "[INFO] Switched render mode to POINTCLOUD.\n";
        break;

    case SDLK_4:
        for (auto& entity : entities) {
            entity->mode = eRenderMode::WIREFRAME;  // Set render mode to WIREFRAME
        }
        std::cout << "[INFO] Switched render mode to WIREFRAME.\n";
        break;*/

        // Scene selection: Toggle between single entity rendering and multiple entities rendering
    case SDLK_1:
        current_scene = 1;  // Switch to rendering a single entity
        std::cout << "[INFO] Switched to single entity rendering.\n";
        break;

    case SDLK_2:
        current_scene = 2;  // Switch to rendering multiple entities
        std::cout << "[INFO] Switched to multiple animated entities.\n";
        break;

        // Adjust camera properties (near, far, FOV)
    case SDLK_n:
        current_property = 'N';
        std::cout << "[INFO] Selected Camera Near property.\n";
        break;

    case SDLK_f:
        current_property = 'F';
        std::cout << "[INFO] Selected Camera Far property.\n";
        break;

    case SDLK_v:
        current_property = 'V';
        std::cout << "[INFO] Selected Camera FOV property.\n";
        break;

    case SDLK_PLUS:
        if (current_property == 'N') {
            camera->near_plane += 0.2f;  // Increase the near plane distance
        }
        if (current_property == 'F') {
            camera->far_plane += 1.0f;  // Increase the far plane distance
        }
        if (current_property == 'V') {
            camera->fov += 2.0f;  // Increase the FOV
        }
        camera->UpdateProjectionMatrix();  // Update projection matrix after changes
        std::cout << "[INFO] Increased current property.\n";
        break;

    case SDLK_MINUS:
        if (current_property == 'N') {
            camera->near_plane = std::max(0.01f, camera->near_plane - 0.2f);  // Decrease near plane, ensure no negative values
        }
        if (current_property == 'F') {
            camera->far_plane = std::max(1.0f, camera->far_plane - 1.0f);  // Decrease far plane, ensure it doesn't go below 1
        }
        if (current_property == 'V') {
            camera->fov = std::max(10.0f, camera->fov - 2.0f);  // Decrease FOV, ensure it doesn't go below 10
        }
        camera->UpdateProjectionMatrix();  // Update projection matrix after changes
        std::cout << "[INFO] Decreased current property.\n";
        break;
    }
}

void Application::OnMouseButtonDown(SDL_MouseButtonEvent event)
{
    if (event.button == SDL_BUTTON_LEFT) {
        mouse_state |= SDL_BUTTON(SDL_BUTTON_LEFT);
    }
    if (event.button == SDL_BUTTON_RIGHT) {
        mouse_state |= SDL_BUTTON(SDL_BUTTON_RIGHT);
    }
}

void Application::OnMouseButtonUp(SDL_MouseButtonEvent event)
{
    if (event.button == SDL_BUTTON_LEFT) {
        mouse_state &= ~SDL_BUTTON(SDL_BUTTON_LEFT);
    }
    if (event.button == SDL_BUTTON_RIGHT) {
        mouse_state &= ~SDL_BUTTON(SDL_BUTTON_RIGHT);
    }
}

void Application::OnMouseMove(SDL_MouseButtonEvent event) {
    // Get current mouse position
    int x = event.x;
    int y = event.y;

    Vector2 current_mouse_position(x, y);  // Store current mouse position
    Vector2 mouse_delta = current_mouse_position - last_mouse_position; // Calculate movement delta

    // Move the center of the camera when the right button is pressed
    if (mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
        float sensitivity = 0.01f;
        camera->center.x += mouse_delta.x * sensitivity;
        camera->center.y += mouse_delta.y * sensitivity; // Inverted for intuitive controls
        camera->UpdateViewMatrix();
    }

    // Orbit the camera when the left button is pressed
    if (mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        float angle_x = mouse_delta.x * 0.0005f; // Sensitivity for horizontal rotation
        float angle_y = mouse_delta.y * 0.005f; // Sensitivity for vertical rotation

        Vector3 direction = camera->eye - camera->center;
        Matrix44 rotation;
        rotation.SetRotation(angle_x, Vector3(0, -1, 0)); // Horizontal rotation
        direction = rotation * direction;

        rotation.SetRotation(angle_y, camera->GetLocalVector(Vector3(1, 0, 0))); // Vertical rotation
        direction = rotation * direction;

        camera->eye = camera->center + direction;
        camera->UpdateViewMatrix();
    }

    // Update last mouse position for next movement
    last_mouse_position = current_mouse_position;
}


void Application::OnWheel(SDL_MouseWheelEvent event)
{
    float dy = event.preciseY; // Obligatorio

    float zoom_factor = 1.1f;
    Vector3 direction = camera->eye - camera->center;

    if (dy > 0) // Zoom in
        direction = direction * (1.0f / zoom_factor);
    else if (dy < 0) // Zoom out
        direction = direction * zoom_factor;

    camera->eye = camera->center + direction;
    camera->UpdateViewMatrix();
}


void Application::OnFileChanged(const char* filename)
{
    Shader::ReloadSingleShader(filename);
}
