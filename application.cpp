#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h"
#include "image.h"

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
}

int Application::ComputeRadius(int x1, int y1, int x2, int y2) {
    return static_cast<int>(sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2)));
}

void Application::Render(void) {
    // Clear the framebuffer

    // Only render when shouldRender is true
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
            framebuffer.DrawCircle(startX, startY, radius, Color(0, 0, 255), 3, false, Color(0, 255, 0));
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
bool isFilled = false;
int borderWidth = 1;
//keyboard press event
void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
    // KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
    switch (event.keysym.sym) {

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


    case SDLK_f:
        if (isFilled == true) {
            isFilled = false;
            std::cout << "Filling OFF" << std::endl;
        }// alternate fill
        else {
            isFilled = true;
            std::cout << "Filling ON" << std::endl;
        }
        break;

    case SDLK_PLUS: // Increase border width
        borderWidth++;
        std::cout << "Border width: " << borderWidth << std::endl;
        break;

    case SDLK_MINUS: // Decrease border width
        borderWidth = std::max(1, borderWidth - 1);
        std::cout << "Border width: " << borderWidth << std::endl;
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



void Application::OnMouseButtonUp(SDL_MouseButtonEvent event) {
    if (event.button == SDL_BUTTON_LEFT) {
        // Example: Logging or cleanup after mouse button release
        std::cout << "Mouse button released at (" << mouse_position.x << ", " << mouse_position.y << ")" << std::endl;
    }
}

void Application::OnMouseButtonDown(SDL_MouseButtonEvent event) {
    if (event.button == SDL_BUTTON_LEFT) {
        if (exercise == 1) { // Draw Line
            if (p1 == NULL) {
                p1 = new Vector2(mouse_position.x, mouse_position.y); // First point
            }
            else {
                p2 = new Vector2(mouse_position.x, mouse_position.y); // Second point
                framebuffer.DrawLineDDA(p1->x, p1->y, p2->x, p2->y, selected_color); // Draw line
                delete p1; // Free memory
                delete p2;
                p1 = NULL;
                p2 = NULL;
            }
        }
        else if (exercise == 2) { // Draw Rectangle
            if (p1 == NULL) {
                p1 = new Vector2(mouse_position.x, mouse_position.y); // First corner
            }
            else {
                p2 = new Vector2(mouse_position.x, mouse_position.y); // Opposite corner
                int width = p2->x - p1->x;
                int height = p2->y - p1->y;
                framebuffer.DrawRect(p1->x, p1->y, width, height, selected_color, borderWidth, isFilled, selected_color); // Draw rectangle
                delete p1; // Free memory
                delete p2;
                p1 = NULL;
                p2 = NULL;
            }
        }
        else if (exercise == 3) { // Draw Circle
            if (p1 == NULL) {
                p1 = new Vector2(mouse_position.x, mouse_position.y); // Center of the circle
            }
            else {
                p2 = new Vector2(mouse_position.x, mouse_position.y); // Point on the perimeter
                int radius = static_cast<int>(sqrt(pow(p2->x - p1->x, 2) + pow(p2->y - p1->y, 2))); // Calculate radius
                framebuffer.DrawCircle(p1->x, p1->y, radius, selected_color, borderWidth, isFilled, selected_color);
                delete p1; // Free memory
                delete p2;
                p1 = NULL;
                p2 = NULL;
            }
        }
        else if (exercise == 4) { // Draw Triangle
            if (p1 == NULL) {
                p1 = new Vector2(mouse_position.x, mouse_position.y); // First vertex
            }
            else if (p2 == NULL) {
                p2 = new Vector2(mouse_position.x, mouse_position.y); // Second vertex
            }
            else {
                Vector2* p3 = new Vector2(mouse_position.x, mouse_position.y); // Third vertex
                framebuffer.RasterizeTriangle(p1->x, p1->y, p2->x, p2->y, p3->x, p3->y, selected_color); // Draw triangle
                delete p1; // Free memory
                delete p2;
                delete p3;
                p1 = NULL;
                p2 = NULL;
            }
        }
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
