#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h"
#include "entity.h"
#include "camera.h"

Application::Application(const char* caption, int width, int height)
{
    this->window = createWindow(caption, width, height);

    int w,h;
    SDL_GetWindowSize(window,&w,&h);

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
    camera = new Camera();
    camera->LookAt(Vector3(0, 0, 3), Vector3(0, 0.25, 0), Vector3(0, -1, 0));
    camera->SetPerspective(45.0f, (float)framebuffer.width / (float)framebuffer.height, 0.1f, 100.0f);

    Mesh* lee = new Mesh();
    if (!lee->LoadOBJ("meshes/lee.obj")) {
        std::cerr << "Failed to load lee.obj!\n";
        return;
    }

    Entity* entity1 = new Entity();
    entity1->mesh = lee;
    entity1->id = 1;
    entity1->model.Translate(-1.5f, 0.0f, 0.0f);
    
    Entity* entity2 = new Entity();
    entity2->mesh = lee;
    entity2->id = 2;
    entity2->model.Translate(1.5f, 0.0f, 0.0f);
    
    Entity* entity3 = new Entity();
    entity3->mesh = lee;
    entity3->id = 3;
    entity3->model.Translate(0.0f, 0.0f, 0.0f);
    
    
    entities.push_back(entity1);
    entities.push_back(entity2);
    entities.push_back(entity3);

    std::cout << "Lee model loaded successfully!\n";
}


// Render one frame
void Application::Render(void)
{
    std::cout << "Rendering frame..." << std::endl; // Debug message

    framebuffer.Fill(Color(0, 0, 0));

    for (Entity* entity : entities) {
        if(entity) {
            Color entityColor = entity->color;

            if (entity->id == 1) {
                entityColor = Color(0, 255, 0);
            } else if (entity->id == 2) {
                entityColor = Color(0, 0, 255);
            } else {
                entityColor = Color(255, 255, 255);
            }

            entity->Render(&framebuffer, camera, entityColor);
            std::cout << "Rendering entity ID " << entity->id << " with color ("
                      << entityColor.r << ", " << entityColor.g << ", " << entityColor.b << ")" << std::endl;
        }
    }

    framebuffer.Render(); 
}





// Called after render
void Application::Update(float seconds_elapsed)
{
    for(Entity* entity : entities){
        if(entity)
            entity->Update(seconds_elapsed);
    }
}


//keyboard press event
void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
    // KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
    bool updated = false; // Ensure variable is declared before switch

    switch (event.keysym.sym) {
        case SDLK_ESCAPE:
            exit(0);
            break; // ESC key, kill the app
        
        case SDLK_n:
            current_property = 'N';
            break;
        
        case SDLK_f:
            current_property = 'F';
            break;
        
        case SDLK_v:
            current_property = 'V';
            break;

        case SDLK_PLUS:
        case SDLK_EQUALS: // Some keyboards use '=' instead of '+'
            if (current_property == 'N') { camera->near_plane += 0.1f; updated = true; }
            if (current_property == 'F') { camera->far_plane += 1.0f; updated = true; }
            if (current_property == 'V') { camera->fov += 1.0f; updated = true; }

            if (updated)
                camera->UpdateProjectionMatrix();
            
            break; // Ensure no fall-through
        
        case SDLK_MINUS:
            if (current_property == 'N') { camera->near_plane = std::max(0.01f, camera->near_plane - 0.1f); updated = true; }
            if (current_property == 'F') { camera->far_plane = std::max(1.0f, camera->far_plane - 1.0f); updated = true; }
            if (current_property == 'V') { camera->fov = std::max(10.0f, camera->fov - 1.0f); updated = true; }

            if (updated)
                camera->UpdateProjectionMatrix();
            
            break;
    }
}

void Application::OnMouseButtonDown( SDL_MouseButtonEvent event )
{
    if (event.button == SDL_BUTTON_LEFT){
        mouse_state |= SDL_BUTTON(SDL_BUTTON_LEFT);
    }
    if (event.button == SDL_BUTTON_RIGHT){
        mouse_state |= SDL_BUTTON(SDL_BUTTON_RIGHT);
    }
}

void Application::OnMouseButtonUp( SDL_MouseButtonEvent event )
{
    if (event.button == SDL_BUTTON_LEFT){
        mouse_state &= ~SDL_BUTTON(SDL_BUTTON_LEFT);
    }
    if (event.button == SDL_BUTTON_RIGHT){
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
        camera->center.y -= mouse_delta.y * sensitivity; // Inverted for intuitive controls
        camera->UpdateViewMatrix();
    }

    // Orbit the camera when the left button is pressed
    if (mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        float angle_x = mouse_delta.x * 0.005f; // Sensitivity for horizontal rotation
        float angle_y = mouse_delta.y * 0.005f; // Sensitivity for vertical rotation

        Vector3 direction = camera->eye - camera->center;
        Matrix44 rotation;
        rotation.SetRotation(angle_x, Vector3(0, 1, 0)); // Horizontal rotation
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
