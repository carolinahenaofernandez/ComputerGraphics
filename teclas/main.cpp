#include "framework/application.h"
#include "framework/utils.h"

int main(int argc, char** argv) {
    // Create the application object
    Application* app = new Application("Computer Graphics", 1280, 720);
    app->Init();

    // SDL Event loop
    SDL_Event event;
    bool running = true;

    while (running) {
        // Process SDL events
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false; // Exit the loop
                    break;

                case SDL_KEYDOWN:
                    app->OnKeyPressed(event.key); // Call your OnKeyPressed handler
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    app->OnMouseButtonDown(event.button); // Call your OnMouseButtonDown handler
                    break;

                case SDL_MOUSEBUTTONUP:
                    app->OnMouseButtonUp(event.button); // Call your OnMouseButtonUp handler
                    break;

                default:
                    break;
            }
        }

        // Call the Render method to update the screen
        app->Render();
    }

    // Clean up
    SDL_Window* window = app->window;
    delete app;

    if (window) {
        SDL_DestroyWindow(window);
    }

    return 0;
}
