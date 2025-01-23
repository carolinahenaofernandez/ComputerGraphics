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
    particleSystem.Init();
    
}

int Application::ComputeRadius(int x1, int y1, int x2, int y2) {
    return static_cast<int>(sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2)));
}

void Application::Render(void) {
    for (const Button& button : buttons) {
            button.Render(framebuffer);
        }
    if (particleSystemActive) {
        particleSystem.Render(&framebuffer);
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
                
            case 5: //Drawing tool
                if (buttonsstate) {
                    for (const Button& button : buttons) {
                        button.Render(framebuffer);
                    }
                }
                break;

                
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
    if (particleSystemActive) {
        particleSystem.Update(seconds_elapsed);
    }

}
bool isFilled = false;
int borderWidth = 1;
bool buttonsstate = false;
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
            
    case SDLK_5:
        exercise = 5;
        buttonsstate = !buttonsstate;
            std::cout << "Exercise 5: Drawing Tool " << (buttonsstate ? "enabled" : "disabled") << std::endl;


        if (buttonsstate) {
            InitButtons();
        }
        else {
            framebuffer.Fill(Color(0, 0, 0));
            buttons.clear();
        }
        break;
    
        case SDLK_6:
        if (particleSystemActive) {
            framebuffer.Fill(Color(0, 0, 0));
        }
        particleSystemActive = !particleSystemActive;
        std::cout << "Particle system " << (particleSystemActive ? "activated" : "deactivated") << std::endl;
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
        // Log the mouse position for debugging
        Vector2 mousePosition(mouse_position.x, mouse_position.y);
        std::cout << "Mouse clicked at (" << mousePosition.x << ", " << mousePosition.y << ")" << std::endl;

        // Iterate through all buttons
        for (size_t i = 0; i < buttons.size(); ++i) {
            if (buttons[i].IsMouseInside(mousePosition)) {
                std::cout << "Button " << i << " clicked!" << std::endl;

                // Handle actions based on the button index
                if (i >= 9 && i <= 15) { // Color buttons (adjust indices as needed)
                    switch (i) {
                        case 9:  selected_color = Color(0, 0, 0);    break; // Black
                        case 10: selected_color = Color(255, 255, 255); break; // White
                        case 11: selected_color = Color(255, 192, 203); break; // Pink
                        case 12: selected_color = Color(255, 255, 0);   break; // Yellow
                        case 13: selected_color = Color(255, 0, 0);     break; // Red
                        case 14: selected_color = Color(0, 0, 255);     break; // Blue
                        case 15: selected_color = Color(0, 255, 255);   break; // Cyan
                    }
                    std::cout << "Color changed to (" << selected_color.r << ", "
                              << selected_color.g << ", " << selected_color.b << ")" << std::endl;
                } else {
                    // Handle other buttons (tools, actions, etc.)
                    switch (i) {
                        case 0: // Clear Image button
                            framebuffer.Fill(Color(0, 0, 0));
                            std::cout << "Framebuffer cleared." << std::endl;
                            break;

                        case 1: // Load Image button
                            framebuffer.LoadTGA("load_image.tga");
                            std::cout << "Image loaded from 'load_image.tga'." << std::endl;
                            break;

                        case 2: // Save Image button
                            framebuffer.SaveTGA("saved_image.tga");
                            std::cout << "Image saved as 'saved_image.tga'." << std::endl;
                            break;

                        case 3: // Eraser button
                            selected_color = Color(0, 0, 0); // Black (eraser effect)
                            exercise = 1;
                            std::cout << "Eraser activated." << std::endl;
                            break;

                        case 4: // Pencil button
                            selected_color = Color(255, 255, 255); // White
                            exercise = 1;
                            std::cout << "Pencil activated." << std::endl;
                            break;

                        case 5: // Line tool button
                            exercise = 1;
                            std::cout << "Line tool activated." << std::endl;
                            break;

                        case 6: // Rectangle tool button
                            exercise = 2;
                            std::cout << "Rectangle tool activated." << std::endl;
                            break;

                        case 7: // Circle tool button
                            exercise = 3;
                            std::cout << "Circle tool activated." << std::endl;
                            break;

                        case 8: // Triangle tool button
                            exercise = 4;
                            std::cout << "Triangle tool activated." << std::endl;
                            break;

                        default:
                            std::cout << "No action assigned to button " << i << "." << std::endl;
                            break;
                    }
                }

                return; // Stop further processing as a button was clicked
            }
        }

        // Handle drawing operations if no button was clicked
        if (exercise == 1) { // Draw Line
            if (!p1) {
                p1 = new Vector2(mouse_position.x, mouse_position.y); // First point
            } else {
                p2 = new Vector2(mouse_position.x, mouse_position.y); // Second point
                framebuffer.DrawLineDDA(p1->x, p1->y, p2->x, p2->y, selected_color); // Draw the line
                delete p1; delete p2;
                p1 = nullptr;
                p2 = nullptr;
            }
        } else if (exercise == 2) { // Draw Rectangle
            if (!p1) {
                p1 = new Vector2(mouse_position.x, mouse_position.y); // First corner
            } else {
                p2 = new Vector2(mouse_position.x, mouse_position.y); // Opposite corner
                int width = p2->x - p1->x;
                int height = p2->y - p1->y;
                framebuffer.DrawRect(p1->x, p1->y, width, height, selected_color, borderWidth, isFilled, selected_color);
                delete p1; delete p2;
                p1 = nullptr;
                p2 = nullptr;
            }
        } else if (exercise == 3) { // Draw Circle
            if (!p1) {
                p1 = new Vector2(mouse_position.x, mouse_position.y); // Circle center
            } else {
                p2 = new Vector2(mouse_position.x, mouse_position.y); // Point on the perimeter
                int radius = static_cast<int>(sqrt(pow(p2->x - p1->x, 2) + pow(p2->y - p1->y, 2))); // Compute radius
                framebuffer.DrawCircle(p1->x, p1->y, radius, selected_color, borderWidth, isFilled, selected_color);
                delete p1; delete p2;
                p1 = nullptr;
                p2 = nullptr;
            }
        } else if (exercise == 4) { // Draw Triangle
            if (!p1) {
                p1 = new Vector2(mouse_position.x, mouse_position.y); // First vertex
            } else if (!p2) {
                p2 = new Vector2(mouse_position.x, mouse_position.y); // Second vertex
            } else {
                Vector2* p3 = new Vector2(mouse_position.x, mouse_position.y); // Third vertex
                framebuffer.DrawTriangle(*p1, *p2, *p3, selected_color, isFilled, selected_color);
                delete p1; delete p2; delete p3;
                p1 = nullptr;
                p2 = nullptr;
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
    const int button_spacing = 10;   // Space between buttons
    const int start_x = 10;          // Starting x-coordinate for buttons
    const int start_y = 10;          // Starting y-coordinate for buttons

    const char* imagePaths[numButtons] = {
        "images/clear.png",
        "images/load.png",
        "images/save.png",
        "images/eraser.png",
        "images/pencil.png",
        "images/line.png",
        "images/rectangle.png",
        "images/circle.png",
        "images/triangle.png",
        "images/black.png",
        "images/white.png",
        "images/pink.png",
        "images/yellow.png",
        "images/red.png",
        "images/blue.png",
        "images/cyan.png"
    };

    int current_x = start_x;
    for (int i = 0; i < numButtons; ++i) {
        Image* buttonImage = new Image();
        if (!buttonImage->LoadPNG(imagePaths[i])) {
            std::cout << "Image not found: " << imagePaths[i] << std::endl;
            delete buttonImage;
            continue;
        }

        // Define position and size for the button
        Vector2 buttonPosition(current_x, start_y);
        Vector2 buttonSize(buttonImage->width, buttonImage->height);

        // Create a button and add it to the buttons list
        buttons.emplace_back(*buttonImage, buttonPosition, buttonSize);

        // Advance the x-coordinate for the next button
        current_x += buttonImage->width + button_spacing;

        // Clean up dynamically allocated image (it gets copied into the Button)
        delete buttonImage;
    }
}

void ParticleSystem::Init() {
    for (int i = 0; i < MAX_PARTICLES; ++i) {
        particles[i].inactive = true; // Start as inactive
    }
}

void ParticleSystem::Update(float dt) {
    const int screenWidth = 1280;  // Screen width
    const int screenHeight = 720; // Screen height

    for (int i = 0; i < MAX_PARTICLES; ++i) {
        if (particles[i].inactive) {
            // Respawn particle if inactive
            particles[i].position = Vector2(rand() % screenWidth, rand() % screenHeight); // Random spawn
            particles[i].velocity = Vector2((rand() % 200 - 100) / 100.0f, (rand() % 200 - 100) / 100.0f); // Random velocity
            particles[i].color = Color(rand() % 256, rand() % 256, rand() % 256); // Random color
            particles[i].ttl = 0.0f; // Reset lifetime
            particles[i].inactive = false;
        }

        // Update active particles
        if (!particles[i].inactive) {
            particles[i].ttl += dt; // Increment lifetime
            if (particles[i].ttl >= 120.0f) { // Reactivate after 120 second
                particles[i].inactive = true;
            }

            // Update position
            particles[i].position.x += particles[i].velocity.x * dt * 100.0f;
            particles[i].position.y += particles[i].velocity.y * dt * 100.0f;

            // Wrap-around bounds
            if (particles[i].position.x < 0) particles[i].position.x += screenWidth;
            if (particles[i].position.x >= screenWidth) particles[i].position.x -= screenWidth;
            if (particles[i].position.y < 0) particles[i].position.y += screenHeight;
            if (particles[i].position.y >= screenHeight) particles[i].position.y -= screenHeight;
        }
    }
}

void ParticleSystem::Render(Image* framebuffer) {
    // Clear the framebuffer to black
    framebuffer->Fill(Color(0, 0, 0));

    const int particleSize = 5; // Size of the particle (width and height of the square)

    // Render all active particles
    for (int i = 0; i < MAX_PARTICLES; ++i) {
        if (!particles[i].inactive) {
            // Draw a square for each particle
            int x = static_cast<int>(particles[i].position.x);
            int y = static_cast<int>(particles[i].position.y);

            // Draw a filled square (use a loop to simulate a larger particle)
            for (int dx = -particleSize / 2; dx <= particleSize / 2; ++dx) {
                for (int dy = -particleSize / 2; dy <= particleSize / 2; ++dy) {
                    int drawX = x + dx;
                    int drawY = y + dy;

                    // Check bounds to avoid out-of-framebuffer access
                    if (drawX >= 0 && drawX < framebuffer->width && drawY >= 0 && drawY < framebuffer->height) {
                        framebuffer->SetPixel(drawX, drawY, particles[i].color);
                    }
                }
            }
        }
    }
}

