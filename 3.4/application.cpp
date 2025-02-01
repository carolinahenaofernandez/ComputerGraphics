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
    std::cout << "Initializing Application..." << std::endl;

    // 📷 Configurar cámara más cerca y con corrección de inversión
    camera.LookAt(Vector3(0, 2, 8), Vector3(0, 1, 0), Vector3(0, 1, 0)); // Mueve la cámara más cerca
    camera.SetPerspective(60.0f, framebuffer.width / (float)framebuffer.height, 0.1f, 50.0f); // Ajusta el FOV
    camera.UpdateViewMatrix();

    std::cout << "Camera initialized.\n";

    // 🎭 Cargar modelos de caras humanas con posiciones corregidas
    std::cout << "Loading human face models...\n";

    Mesh* face1 = new Mesh();
    if (!face1->LoadOBJ("meshes/lee.obj")) {
        std::cerr << "Failed to load face1.obj!\n";
    } else {
        face1->name = "Entity1";
        Entity entity1(face1);
        entity1.model.Translate(-2.5f, 1.0f, 0); // Mueve la cara más arriba y centrada
        entity1.model.Rotate(180 * DEG2RAD, Vector3(1, 0, 0)); // Corrige la inversión
        entities.push_back(entity1);
    }

    Mesh* face2 = new Mesh();
    if (!face2->LoadOBJ("meshes/lee.obj")) {
        std::cerr << "Failed to load face2.obj!\n";
    } else {
        face2->name = "Entity2";
        Entity entity2(face2);
        entity2.model.Translate(0.0f, 1.0f, 0); // Centrar la cara
        entity2.model.Rotate(180 * DEG2RAD, Vector3(1, 0, 0)); // Corrige la inversión
        entities.push_back(entity2);
    }

    Mesh* face3 = new Mesh();
    if (!face3->LoadOBJ("meshes/lee.obj")) {
        std::cerr << "Failed to load face3.obj!\n";
    } else {
        face3->name = "Entity3";
        Entity entity3(face3);
        entity3.model.Translate(2.5f, 1.0f, 0); // Mueve la cara a la derecha
        entity3.model.Rotate(180 * DEG2RAD, Vector3(1, 0, 0)); // Corrige la inversión
        entities.push_back(entity3);
    }

    std::cout << "Total entities: " << entities.size() << "\n";
}




void Application::Render() {
    framebuffer.Fill(Color(0, 0, 0)); // Limpiar framebuffer a negro

    for (size_t i = 0; i < entities.size(); i++) {
        Color entityColor;

        // 🎨 Asignar colores diferentes a cada entidad
        if (entities[i].mesh->name == "Entity1") {
            entityColor = Color(255, 0, 0); // Rojo
        } else if (entities[i].mesh->name == "Entity2") {
            entityColor = Color(0, 0, 255); // Azul
        } else {
            entityColor = Color(255, 255, 255); // Blanco
        }

        entities[i].Render(&framebuffer, &camera, entityColor);
    }

    framebuffer.Render(); // Mostrar el framebuffer en pantalla
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
    float speed = 0.5f;
        Vector3 moveDir;
	switch(event.keysym.sym){
        case SDLK_w: moveDir = Vector3(0, 0, -speed); break; // Avanzar
        case SDLK_s: moveDir = Vector3(0, 0, speed); break;  // Retroceder
        case SDLK_a: moveDir = Vector3(-speed, 0, 0); break; // Izquierda
        case SDLK_d: moveDir = Vector3(speed, 0, 0); break;  // Derecha
		case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app
	}
    camera.Move(moveDir);
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
