#include "camera.h"

class Application
{
public:

// Window

    //SDL_Window* window = nullptr;
    //int window_width;
    //int window_height;

//!!!Sol has de ficar estes tres linies
    float time;
    std::vector<Entity*> entities;
    Camera* camera;
    float current_property; // Valor actual de la propiedad modificada (near, far, FOV)
    char property_mode; // 'N' = near, 'F' = far, 'V' = FOV
    Vector2 last_mouse_position;

};
