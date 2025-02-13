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

/*void Application::Init(void)
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
    
    texture_color_specular = new Image();
    if (!texture_color_specular->LoadTGA("textures/lee_color_specular.tga")) {
        std::cerr << "Failed to load lee_color_specular.tga!\n";
        return;
    }
    
    texture_normal = new Image();
    if (!texture_normal->LoadTGA("texture/lee_normal.tga")) {
        std::cerr << "Failed to load lee_normal.tga!\n";
        return;
    }
    
    zBuffer.Resize(framebuffer.width, framebuffer.height);

    Entity* entity1 = new Entity();
    entity1->mesh = lee;
    entity1->id = 1;
    entity1->texture = texture_normal;
    entity1->model.Translate(-1.5f, 0.0f, 0.0f);
    
    Entity* entity2 = new Entity();
    entity2->mesh = lee;
    entity2->id = 2;
    entity2->texture = texture_normal;
    entity2->model.Translate(1.5f, 0.0f, 0.0f);
    
    Entity* entity3 = new Entity();
    entity3->mesh = lee;
    entity3->id = 3;
    entity3->texture = nullptr;
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

    framebuffer.Fill(Color(0, 0, 0)); // Clear framebuffer
    zBuffer.Fill(1.0f); // Initialize Z-buffer with maximum depth (1.0 means farthest)

    if (current_scene == 1) // Draw a single entity
    {
        if (!entities.empty() && entities[0]) // Ensure there's at least one entity
        {
            Entity* entity = entities[2];
            Color entityColor = Color(255, 255, 255); // Default color

            if (entity->id == 1) {
                entityColor = Color(0, 255, 0); // Green
            } else if (entity->id == 2) {
                entityColor = Color(0, 0, 255); // Blue
            }

            // FIX: Now pass `&zBuffer` instead of `entityColor`
            entity->Render(&framebuffer, camera, &zBuffer);
            std::cout << "Rendering single entity ID " << entity->id << " with color ("
                      << entityColor.r << ", " << entityColor.g << ", " << entityColor.b << ")" << std::endl;
        }
    }
    else if (current_scene == 2) // Draw multiple animated entities
    {
        for (Entity* entity : entities) {
            if (entity) {
                Color entityColor = Color(255, 255, 255); // Default color

                if (entity->id == 1) {
                    entityColor = Color(0, 255, 0); // Green
                } else if (entity->id == 2) {
                    entityColor = Color(0, 0, 255); // Blue
                }

                // FIX: Now pass `&zBuffer` instead of `entityColor`
                entity->Render(&framebuffer, camera, &zBuffer);
                std::cout << "Rendering entity ID " << entity->id << " with color ("
                          << entityColor.r << ", " << entityColor.g << ", " << entityColor.b << ")" << std::endl;
            }
        }
    }

    framebuffer.Render();
}
void Application::Init(void)
{
    std::cout << "Initiating app..." << std::endl;

    // Initialize the camera
    camera = new Camera();
    camera->LookAt(Vector3(0, 0, 3), Vector3(0, 0.25, 0), Vector3(0, -1, 0));
    camera->SetPerspective(45.0f, (float)framebuffer.width / (float)framebuffer.height, 0.1f, 100.0f);

    // Load the mesh
    Mesh* lee = new Mesh();
    if (!lee->LoadOBJ("meshes/lee.obj")) {
        std::cerr << "[ERROR] Failed to load lee.obj!\n";
        return;
    } else {
        std::cout << "[INFO] Mesh lee.obj loaded successfully!\n";
    }

    // Resize Z-buffer
    zBuffer.Resize(framebuffer.width, framebuffer.height);
    Image* textureImage = new Image();
        if (!textureImage->LoadTGA("textures/lee_color_specular.tga",true)) {
            std::cerr << "Failed to load texture!" << std::endl;
            return;
        }

    Image* normalImage = new Image();
        if (!normalImage->LoadTGA("textures/lee_normal.tga",true)) {
            std::cerr << "Failed to load normal map!" << std::endl;
            return;
        }
    
    // Create and configure entities
    for (int i = 0; i < 3; i++) {
        Entity* entity = new Entity();
        entity->mesh = lee;
        entity->texture = textureImage;
        entity->normalMap = normalImage;
        entity->id = i + 1;

        if (i == 0) entity->model.Translate(-1.5f, 0.0f, 0.0f);
        if (i == 1) entity->model.Translate(1.5f, 0.0f, 0.0f);
        if (i == 2) entity->model.Translate(0.0f, 0.0f, 0.0f);

        entities.push_back(entity);
        std::cout << "[INFO] Created Entity " << entity->id << " with Mesh and Textures.\n";
    }

    std::cout << "Lee model and textures loaded successfully!\n";
}

void Application::Render(void)
{
    std::cout << "Rendering frame..." << std::endl; // Debug message

    // Clear framebuffer and reset Z-buffer
    framebuffer.Fill(Color(0, 0, 0)); // Black background
    zBuffer.Fill(1.0f); // Reset depth buffer (1.0 means farthest)

    // Debug: Check if there are entities
    if (entities.empty()) {
        std::cerr << "[ERROR] No entities to render!\n";
        return;
    }

    if (current_scene == 1) // Draw a single entity
    {
        if (!entities.empty() && entities[2]) // Ensure there's at least one entity
        {
            Entity* entity = entities[2];
            if (!entity->mesh) {
                std::cerr << "[ERROR] Entity " << entity->id << " has no mesh!\n";
                return;
            }

            std::cout << "[INFO] Rendering single entity ID " << entity->id << std::endl;
            entity->Render(&framebuffer, camera, &zBuffer);
        }
    }
    else if (current_scene == 2) // Draw multiple entities
    {
        for (Entity* entity : entities) {
            if (!entity) {
                std::cerr << "[ERROR] Null entity in entities list!\n";
                continue;
            }
            if (!entity->mesh) {
                std::cerr << "[ERROR] Entity " << entity->id << " has no mesh!\n";
                continue;
            }

            std::cout << "[INFO] Rendering entity ID " << entity->id << std::endl;
            entity->Render(&framebuffer, camera, &zBuffer);
        }
    }

    // Debug: Confirm framebuffer is being updated
    std::cout << "[INFO] Finished rendering frame, calling framebuffer.Render()\n";
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
    bool updated = false; // Para verificar si necesitamos actualizar la proyección
    
    switch (event.keysym.sym) {
        case SDLK_ESCAPE:
            exit(0);
            break;
            
            // Toggle between MESH TEXTURE and COLOR PER VERTEX
        case SDLK_t:
            for (auto& entity : entities) {
                entity->texture = (entity->texture == nullptr) ? texture_color_specular : nullptr;
            }
            std::cout << "[INFO] Toggled between mesh texture and vertex color.\n";
            break;
            
            // Toggle between OCCLUSIONS (Z-Buffer) and NO OCCLUSIONS
        case SDLK_z:
            for (auto& entity : entities) {
                entity->useZBuffer = !entity->useZBuffer;  // Assuming useZBuffer is a boolean in Entity
            }
            std::cout << "[INFO] Toggled occlusions (Z-Buffer).\n";
            break;
            
            // Toggle between INTERPOLATED UVs and PLAIN COLOR
        case SDLK_c:
            for (auto& entity : entities) {
                if (entity->mode == eRenderMode::TRIANGLES) {
                    entity->mode = eRenderMode::TRIANGLES_INTERPOLATED;
                } else if (entity->mode == eRenderMode::TRIANGLES_INTERPOLATED) {
                    entity->mode = eRenderMode::TRIANGLES;
                }
            }
            std::cout << "[INFO] Toggled between interpolated UVs and plain color.\n";
            break;
            
            // Scene selection: Single Entity or Multiple Animated Entities
        case SDLK_1:
            current_scene = 1;
            std::cout << "[INFO] Switched to single entity rendering.\n";
            break;
            
        case SDLK_2:
            current_scene = 2;
            std::cout << "[INFO] Switched to multiple animated entities.\n";
            break;
            
            // Adjusting Camera Properties
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
        case SDLK_EQUALS: // Some keyboards use '=' for '+'
            if (current_property == 'N') {
                camera->near_plane += 0.1f;
            }
            if (current_property == 'F') {
                camera->far_plane += 1.0f;
            }
            if (current_property == 'V') {
                camera->fov += 1.0f;
            }
            camera->UpdateProjectionMatrix();
            std::cout << "[INFO] Increased current property.\n";
            break;
            
        case SDLK_MINUS:
            if (current_property == 'N') {
                camera->near_plane = std::max(0.01f, camera->near_plane - 0.1f);
            }
            if (current_property == 'F') {
                camera->far_plane = std::max(1.0f, camera->far_plane - 1.0f);
            }
            if (current_property == 'V') {
                camera->fov = std::max(10.0f, camera->fov - 1.0f);
            }
            camera->UpdateProjectionMatrix();
            std::cout << "[INFO] Decreased current property.\n";
            break;
    }
}*/
void Application::Init(void)
{
    std::cout << "Initiating app..." << std::endl;

    // Initialize the camera
    camera = new Camera();
    camera->LookAt(Vector3(0, 0, 3), Vector3(0, 0.25, 0), Vector3(0, -1, 0));
    camera->SetPerspective(45.0f, (float)framebuffer.width / (float)framebuffer.height, 0.1f, 100.0f);

    // Load the mesh
    Mesh* lee = new Mesh();
    if (!lee->LoadOBJ("meshes/lee.obj")) {
        std::cerr << "[ERROR] Failed to load lee.obj!\n";
        return;
    } else {
        std::cout << "[INFO] Mesh lee.obj loaded successfully!\n";
    }

    // Load textures
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

    zBuffer.Resize(framebuffer.width, framebuffer.height);

    // Create and configure entities
    for (int i = 0; i < 3; i++) {
        Entity* entity = new Entity();
        entity->mesh = lee;
        entity->texture = texture_color_specular;
        entity->normalMap = texture_normal;
        entity->id = i + 1;

        if (i == 0) entity->model.Translate(-1.5f, 0.0f, 0.0f);
        if (i == 1) entity->model.Translate(1.5f, 0.0f, 0.0f);
        if (i == 2) entity->model.Translate(0.0f, 0.0f, 0.0f);

        entities.push_back(entity);
        std::cout << "[INFO] Created Entity " << entity->id << " with Mesh and Textures.\n";
    }

    std::cout << "Lee model and textures loaded successfully!\n";
}

void Application::Render(void)
{
    std::cout << "Rendering frame..." << std::endl; // Debug message

    framebuffer.Fill(Color(0, 0, 0));  // Clear the framebuffer

    // Initialize Z-buffer (if not done already)
    FloatImage zBuffer(framebuffer.width, framebuffer.height);
    for (int y = 0; y < framebuffer.height; ++y) {
        for (int x = 0; x < framebuffer.width; ++x) {
            zBuffer.SetPixel(x, y, 1.0f); // Initialize Z-buffer to the farthest distance (1.0)
        }
    }

    // Check which scene to render: Single entity (scene 1) or multiple entities (scene 2)
    if (current_scene == 1) // Render a single entity
    {
        if (!entities.empty() && entities[2]) // Ensure there's at least one entity
        {
            Entity* entity = entities[2];  // Select the entity (in this case with id=3)
            Color entityColor = Color(255, 255, 255);  // Default color

            // Assign colors based on entity id
            if (entity->id == 1) {
                entityColor = Color(0, 255, 0);  // Green
            }
            else if (entity->id == 2) {
                entityColor = Color(0, 0, 255);  // Blue
            }

            // Call the Render function to draw the entity
            // Use RenderLab3 with Z-buffer if necessary
            if (isLab3) {
                entity->RenderLab3(&framebuffer, camera, &zBuffer);
            } else {
                entity->RenderLab2(&framebuffer, camera, entityColor);
            }

            std::cout << "Rendering single entity ID " << entity->id << " with color ("
                      << entityColor.r << ", " << entityColor.g << ", " << entityColor.b << ")" << std::endl;
        }
    }
    else if (current_scene == 2) // Render multiple animated entities
    {
        for (Entity* entity : entities) {
            if (entity) {
                Color entityColor = Color(255, 255, 255);  // Default color

                // Assign colors based on entity id
                if (entity->id == 1) {
                    entityColor = Color(0, 255, 0);  // Green
                }
                else if (entity->id == 2) {
                    entityColor = Color(0, 0, 255);  // Blue
                }

                // Use the same logic to handle Z-buffer for multiple entities
                if (isLab3) {
                    entity->RenderLab3(&framebuffer, camera, &zBuffer);
                } else {
                    entity->RenderLab2(&framebuffer, camera, entityColor);
                }

                std::cout << "Rendering entity ID " << entity->id << " with color ("
                          << entityColor.r << ", " << entityColor.g << ", " << entityColor.b << ")" << std::endl;
            }
        }
    }

    // Render the final image
    framebuffer.Render();
}


void Application::Update(float seconds_elapsed)
{
    for(Entity* entity : entities){
        if(entity)
            entity->Update(seconds_elapsed);
    }
}
void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
    bool updated = false;

    switch (event.keysym.sym) {
        case SDLK_ESCAPE:
            exit(0);
            break;

        // Toggle between MESH TEXTURE and COLOR PER VERTEX
        case SDLK_t:
            for (auto& entity : entities) {
                // Toggle between texture and color
                entity->texture = (entity->texture == nullptr) ? texture_color_specular : nullptr;
            }
            std::cout << "[INFO] Toggled between mesh texture and vertex color.\n";
            break;

        // Toggle between OCCLUSIONS (Z-Buffer) and NO OCCLUSIONS
        case SDLK_z:
            for (auto& entity : entities) {
                entity->useZBuffer = !entity->useZBuffer;  // Toggle Z-buffer
            }
            std::cout << "[INFO] Toggled occlusions (Z-Buffer).\n";
            break;

        // Toggle between INTERPOLATED UVs and PLAIN COLOR
        case SDLK_c:
            for (auto& entity : entities) {
                if (entity->mode == eRenderMode::TRIANGLES) {
                    entity->mode = eRenderMode::TRIANGLES_INTERPOLATED;  // Switch to interpolated mode
                } else if (entity->mode == eRenderMode::TRIANGLES_INTERPOLATED) {
                    entity->mode = eRenderMode::TRIANGLES;  // Switch back to plain triangles
                }
            }
            std::cout << "[INFO] Toggled between interpolated UVs and plain color.\n";
            break;
        
        case SDLK_s:  // Alternar entre estático y animado (si quieres que las entidades estén quietas o animadas)
            for (auto& entity : entities) {
                entity->is_moving = !entity->is_moving;  // Alternar el estado de la entidad
            }
                std::cout << "[INFO] Alternando entre entidades estáticas y animadas." << std::endl;
            break;
            
        case SDLK_l:  // Toggle Lab 2 (Wireframe)
            isLab3 = false;
            std::cout << "Switched to Lab 2 (Wireframe mode)" << std::endl;
            break;

        case SDLK_k:  // Toggle Lab 3 (Z-buffer, Interpolated Triangles)
            isLab3 = true;
            std::cout << "Switched to Lab 3 (Z-buffer mode)" << std::endl;
            break;
    

        // Switch between different render modes (POINTCLOUD, WIREFRAME, TRIANGLES, TRIANGLES_INTERPOLATED)
        case SDLK_1:
            for (auto& entity : entities) {
                entity->mode = eRenderMode::POINTCLOUD;
            }
            std::cout << "[INFO] Switched render mode to POINTCLOUD.\n";
            break;

        case SDLK_2:
            for (auto& entity : entities) {
                entity->mode = eRenderMode::WIREFRAME;
            }
            std::cout << "[INFO] Switched render mode to WIREFRAME.\n";
            break;

        case SDLK_3:
            for (auto& entity : entities) {
                entity->mode = eRenderMode::TRIANGLES;
            }
            std::cout << "[INFO] Switched render mode to TRIANGLES.\n";
            break;

        case SDLK_4:
            for (auto& entity : entities) {
                entity->mode = eRenderMode::TRIANGLES_INTERPOLATED;
            }
            std::cout << "[INFO] Switched render mode to TRIANGLES_INTERPOLATED.\n";
            break;

        // Scene selection: Single Entity or Multiple Animated Entities
        case SDLK_5:
            current_scene = 1;
            std::cout << "[INFO] Switched to single entity rendering.\n";
            break;

        case SDLK_6:
            current_scene = 2;
            std::cout << "[INFO] Switched to multiple animated entities.\n";
            break;

        // Adjusting Camera Properties
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
                case SDLK_EQUALS:  // Algunas teclas para aumentar el FOV
                    camera->fov += 1.0f;
                    camera->UpdateProjectionMatrix();  // Recalcular la matriz de proyección con el nuevo FOV
                    std::cout << "Campo de visión aumentado: " << camera->fov << std::endl;
                    break;
                
                case SDLK_MINUS:  // Disminuir el FOV
                    camera->fov = std::max(10.0f, camera->fov - 1.0f);  // Evitar valores de FOV demasiado pequeños
                    camera->UpdateProjectionMatrix();  // Recalcular la matriz de proyección con el nuevo FOV
                    std::cout << "Campo de visión disminuido: " << camera->fov << std::endl;
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
