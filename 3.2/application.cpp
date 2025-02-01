#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h"
#include "entity.h"
#include "camera.h"
#include <iostream>

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

void Application::Init() {
    std::cout << "Initiating app..." << std::endl;

    // Load a mesh (e.g., a cube)
    Mesh* myMesh = new Mesh();
    myMesh->CreateCube(1.0f);

    // Assign the mesh to the class member `myEntity` (not a local variable)
    myEntity = Entity(myMesh);

    // Initialize camera (set its position and target correctly)
    camera.LookAt(Vector3(0, 0, 5),  // Camera position (moved from (0,1,5) to (0,0,5))
                  Vector3(0, 0, 0),  // Look at the origin
                  Vector3(0, 1, 0)); // Up direction

    // Print confirmation
    std::cout << "Entity created with a cube mesh." << std::endl;
    
    // Load meshes
    Mesh* mesh1 = new Mesh();
    mesh1->CreateCube(1.0f);
    mesh1->name = "Entity1";

    Mesh* mesh2 = new Mesh();
    mesh2->CreateCube(1.0f);
    mesh2->name = "Entity2";

    Mesh* mesh3 = new Mesh();
    mesh3->CreateCube(1.0f);
    mesh3->name = "Entity3";

    // Create entities with different meshes
    entities.push_back(Entity(mesh1));
    entities.push_back(Entity(mesh2));
    entities.push_back(Entity(mesh3));

    std::cout << "Three animated entities created!" << std::endl;

    // Set proper perspective projection
    camera.SetPerspective(45.0f, framebuffer.width / (float)framebuffer.height, 0.1f, 50.0f);
    
    // Ensure the view matrix updates
    camera.UpdateViewMatrix();

    // Debugging: Print camera settings
    std::cout << "Camera initialized: " << std::endl;
    std::cout << "Eye: " << camera.eye.x << ", " << camera.eye.y << ", " << camera.eye.z << std::endl;
    std::cout << "Center: " << camera.center.x << ", " << camera.center.y << ", " << camera.center.z << std::endl;
    std::cout << "Up: " << camera.up.x << ", " << camera.up.y << ", " << camera.up.z << std::endl;
}

void Application::Render() {
    Color wireframeColor = Color(255, 255, 255); // White wireframe
    framebuffer.Fill(Color(0, 0, 0)); // Clear framebuffer to black before drawing

    myEntity.Render(&framebuffer, &camera, wireframeColor);
    
    for (Entity& entity : entities) {
        entity.Render(&framebuffer, &camera, wireframeColor);
    }

    framebuffer.Render(); // Display the framebuffer
}


// Called after render
void Application::Update(float seconds_elapsed)
{
    for (Entity& entity : entities) {
            entity.Update(seconds_elapsed);
        }
}

//keyboard press event 
void Application::OnKeyPressed( SDL_KeyboardEvent event )
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch(event.keysym.sym) {
		case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app
	}
}

void Application::OnMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) {

	}
}

void Application::OnMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) {

	}
}

void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
	
}

void Application::OnWheel(SDL_MouseWheelEvent event)
{
	float dy = event.preciseY;

	// ...
}

void Application::OnFileChanged(const char* filename)
{ 
	Shader::ReloadSingleShader(filename);
}
