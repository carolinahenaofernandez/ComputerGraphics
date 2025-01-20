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

int Application::ComputeRadius(int x1, int y1, int x2, int y2) {
    return static_cast<int>(sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2)));
}

void Application::Render(void) {
    // Clear the framebuffer
    framebuffer.Fill(Color::BLACK);

    // Only render when `shouldRender` is true
    if (shouldRender) {
        switch (exercise) {
            case 1: // Draw Line
                framebuffer.DrawLineDDA(startX, startY, endX, endY, Color(255, 255, 255)); // White line
                break;

            case 2: // Draw Rectangle
                framebuffer.DrawRect(startX, startY, abs(endX - startX), abs(endY - startY), Color(255, 0, 0), 3, true, Color(0, 255, 0)); // Red border, green fill
                break;

            case 3: { // Start a new scope for this case
                int radius = ComputeRadius(startX, startY, endX, endY); // Calculate radius
                framebuffer.RasterizeCircle(startX, startY, radius, Color(0, 0, 255)); // Blue circle
                break;
            }

            case 4: { // Start a new scope for this case
                framebuffer.RasterizeTriangle(startX, startY, (startX + endX) / 2, endY, endX, startY, Color(255, 255, 0)); // Yellow triangle
                break;
            }

            default:
                std::cout << "No action for exercise " << exercise << std::endl;
                break;
        }

        // Reset the render flag
        shouldRender = false;
    }

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
		
        case SDLK_1:
            exercise = 1; // Draw Line
            std::cout << "Exercise 1: Draw Line" << std::endl;
            break;

        case SDLK_2:
            exercise = 2; // Draw Rectangle
            std::cout << "Exercise 2: Draw Rectangle" << std::endl;
            break;

        case SDLK_3:
            exercise = 3; // Draw Circle
            std::cout << "Exercise 3: Draw Circle" << std::endl;
            break;

        case SDLK_ESCAPE:
            exit(0); // Exit the program
            break;

        default:
            exercise = 0; // No valid exercise
            std::cout << "No action for this key" << std::endl;
            break;
    }

}


void Application::OnMouseButtonDown(SDL_MouseButtonEvent event) {
    if (event.button == SDL_BUTTON_LEFT) {
        startX = event.x;  // Save starting point
        startY = event.y;
    }
}

void Application::OnMouseButtonUp(SDL_MouseButtonEvent event) {
    if (event.button == SDL_BUTTON_LEFT) {
        endX = event.x;    // Save ending point
        endY = event.y;

        // Indicate that the rendering should happen in the next frame
        shouldRender = true;
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

