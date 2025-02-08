#include "entity.h"
#include "camera.h"

void Application::Init(void)
{
    std::cout << "Initiating app..." << std::endl;
    camera = new Camera();
    camera->LookAt(Vector3(0, 0, 3), Vector3(0, 0.25, 0), Vector3(0, -1, 0));
    camera->SetPerspective(45.0f, (float)framebuffer.width / (float)framebuffer.height, 0.1f, 100.0f);

    Mesh* lee = new Mesh();
    if (!lee->LoadOBJ("meshes/lee.obj")) {
        std::cerr << "Failed to load lee.obj!\n";
        return;
    }

    Entity* entity1 = new Entity();
    entity1->mesh = lee;
    entity1->color = Color(0,255,0);
    entity1->id = 1;
    entity1->model.Translate(-1.5f, 0.0f, 0.0f);
    
    Entity* entity2 = new Entity();
    entity2->mesh = lee;
    entity2->color = Color(0,0,255);
    entity2->id = 2;
    entity2->model.Translate(1.5f, 0.0f, 0.0f);
    
    Entity* entity3 = new Entity();
    entity3->mesh = lee;
    entity3->color = Color(255,255,255);
    entity3->id = 3;
    entity3->model.Translate(0.0f, 0.0f, 0.0f);
    
    entity1->model = Matrix44();
    entity2->model = Matrix44();
    entity3->model = Matrix44();

    entities.push_back(entity1);
    entities.push_back(entity2);
    entities.push_back(entity3);

    std::cout << "Lee model loaded successfully!\n";
}


// Render one frame
void Application::Render(void)
{
    std::cout << "Rendering frame..." << std::endl; // Debug message

    framebuffer.Fill(Color(0, 0, 0));

    for (Entity* entity : entities) {
        if(entity) {
            Color entityColor = entity->color;

            if (entity->id == 1) {
                entityColor = Color(0, 255, 0);
            } else if (entity->id == 2) {
                entityColor = Color(0, 0, 255);
            } else {
                entityColor = Color(255, 255, 255);
            }

            entity->Render(&framebuffer, camera, entityColor);
            std::cout << "Rendering entity ID " << entity->id << " with color ("
                      << entityColor.r << ", " << entityColor.g << ", " << entityColor.b << ")" << std::endl;
        }
    }

    framebuffer.Render(); 
}

// Called after render
void Application::Update(float seconds_elapsed)
{
    for(Entity* entity : entities){
        if(entity)
            entity->Update(seconds_elapsed);
    }
}
