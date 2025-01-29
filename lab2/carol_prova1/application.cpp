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

    // Initialize camera (set its position and target)
    camera.LookAt(Vector3(0, 1, 5), Vector3(0, 0, 0), Vector3(0, 1, 0));

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
}

void Application::Render() {


    Color wireframeColor = Color(255, 255, 255); // White wireframe

    // Render the entity using the camera
    myEntity.Render(&framebuffer, &camera, wireframeColor);
    
    for (Entity& entity : entities) {
            entity.Render(&framebuffer, &camera, wireframeColor);
        }

    // Display the framebuffer
    framebuffer.Render();
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
