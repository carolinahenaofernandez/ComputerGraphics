#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h" 

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
}

// Render one frame
void Application::Render(void)
{
    // Clear the framebuffer once
    framebuffer.Fill(Color::BLACK);

    // Draw a line using the DDA algorithm
    int x0 = 50, y0 = 50;   // Starting point
    int x1 = 150, y1 = 100; // Ending point
    Color lineColor(255, 255, 255); // White color
    framebuffer.DrawLineDDA(x0, y0, x1, y1, lineColor);

    // Draw a filled rectangle with a red border
    framebuffer.DrawRect(60, 110, 100, 60, Color(255, 0, 0), 3, true, Color(0, 255, 0));

    // Draw a non-filled rectangle with a blue border
    framebuffer.DrawRect(200, 100, 120, 80, Color(0, 0, 255), 5);
    // Draw a triangle
    framebuffer.RasterizeTriangle(50, 5, 150, 55, 100, 105, Color(255, 255, 0)); // Yellow triangle
    // Draw a circle centered at (150, 150) with a radius of 50
    framebuffer.RasterizeCircle(150, 150, 50, Color(0, 255, 255)); // Cyan circle

    // Render the framebuffer to the screen
    framebuffer.Render();
}


// Called after render
void Application::Update(float seconds_elapsed)
{

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
