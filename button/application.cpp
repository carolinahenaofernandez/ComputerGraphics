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
    InitButtons();
}

int Application::ComputeRadius(int x1, int y1, int x2, int y2) {
    return static_cast<int>(sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2)));
}

void Application::Render(void) {
    for (const Button& button : buttons) {
            button.Render(framebuffer);
        }

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

        case 4: { // Draw Triangle
            framebuffer.DrawTriangle(Vector2(startX, startY), Vector2((startX + endX) / 2, endY), Vector2(endX, startY), Color(255, 255, 0), false, Color(0, 255, 0));
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
        
    case SDLK_4:
        exercise = 4; // Draw Circle
        std::cout << "Exercise 4: Draw Triangle" << std::endl;
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
        for (size_t i = 0; i < buttons.size(); ++i) {
            if (buttons[i].IsMouseInside(mouse_position)) {
                std::cout << "Button " << i << " clicked!" << std::endl;

                // Assign actions based on the button index
                switch (i) {
                    case 0: // Line button
                        exercise = 1;
                        break;
                    case 1: // Rectangle button
                        exercise = 2;
                        break;
                    case 2: // Circle button
                        exercise = 3;
                        break;
                    case 3: // Triangle button
                        exercise = 4;
                        break;
                    case 4: // Save Image button
                        framebuffer.SaveTGA("saved_image.tga");
                        std::cout << "Image saved as 'saved_image.tga'" << std::endl;
                        break;
                    case 5: // Load Image button
                        framebuffer.LoadTGA("load_image.tga"); // Replace with your desired filename
                        std::cout << "Image loaded from 'load_image.tga'" << std::endl;
                        break;
                    case 6: // Clear Image button
                        framebuffer.Fill(Color(0, 0, 0)); // Clear framebuffer to black
                        exercise = 1; // Set default tool to Line
                        std::cout << "Framebuffer cleared. Default tool set to Line." << std::endl;
                        break;



                    case 7: // Eraser button
                        selected_color = Color(0, 0, 0); // Set color to black (eraser effect)
                        exercise = 1; // Use the line tool for erasing
                        std::cout << "Eraser activated" << std::endl;
                        break;
                    default:
                        break;
                }
                return; // Stop further processing as a button was clicked
            }
        }
        
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
            } else if (p2 == NULL) {
                p2 = new Vector2(mouse_position.x, mouse_position.y); // Second vertex
            } else {
                Vector2* p3 = new Vector2(mouse_position.x, mouse_position.y); // Third vertex
                framebuffer.DrawTriangle(*p1, *p2, *p3, selected_color, isFilled, selected_color); // Draw triangle
                delete p1; delete p2; delete p3; // Free memory
                p1 = NULL; p2 = NULL;
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

Color Application::ChooseRandomColor() {
    return Color(rand() % 256, rand() % 256, rand() % 256); // Random RGB values
}

void Application::InitButtons() {
    const int button_size = 32;   // Size of each button
    const int spacing = 20;      // Space between buttons
    const int start_x = 10;      // Starting x-coordinate for buttons
    const int start_y = 10;      // Starting y-coordinate for buttons

    // Line button
    Image lineIcon(button_size, button_size);
    lineIcon.Fill(Color(0, 0, 255)); // Blue for the line button
    buttons.emplace_back(lineIcon, Vector2(start_x, start_y), Vector2(button_size, button_size));

    // Rectangle button
    Image rectIcon(button_size, button_size);
    rectIcon.Fill(Color(0, 255, 0)); // Green for the rectangle button
    buttons.emplace_back(rectIcon, Vector2(start_x + (button_size + spacing) * 1, start_y), Vector2(button_size, button_size));

    // Circle button
    Image circleIcon(button_size, button_size);
    circleIcon.Fill(Color(255, 255, 0)); // Yellow for the circle button
    buttons.emplace_back(circleIcon, Vector2(start_x + (button_size + spacing) * 2, start_y), Vector2(button_size, button_size));

    // Triangle button
    Image triangleIcon(button_size, button_size);
    triangleIcon.Fill(Color(255, 0, 255)); // Magenta for the triangle button
    buttons.emplace_back(triangleIcon, Vector2(start_x + (button_size + spacing) * 3, start_y), Vector2(button_size, button_size));

    // Save Image button
    Image saveIcon(button_size, button_size);
    saveIcon.Fill(Color(255, 165, 0)); // Orange for the save button
    buttons.emplace_back(saveIcon, Vector2(start_x + (button_size + spacing) * 4, start_y), Vector2(button_size, button_size));

    // Load Image button
    Image loadIcon(button_size, button_size);
    loadIcon.Fill(Color(128, 0, 128)); // Purple for the load button
    buttons.emplace_back(loadIcon, Vector2(start_x + (button_size + spacing) * 5, start_y), Vector2(button_size, button_size));

    // Clear Image button
    Image clearIcon(button_size, button_size);
    clearIcon.Fill(Color(192, 192, 192)); // Gray for the clear button
    buttons.emplace_back(clearIcon, Vector2(start_x + (button_size + spacing) * 6, start_y), Vector2(button_size, button_size));

    // Eraser button
    Image eraserIcon(button_size, button_size);
    eraserIcon.Fill(Color(255, 20, 147)); // Pink for the eraser button
    buttons.emplace_back(eraserIcon, Vector2(start_x + (button_size + spacing) * 7, start_y), Vector2(button_size, button_size));

    // Add other buttons as needed
}
