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
    zBuffer.Resize(framebuffer.width, framebuffer.height);

    Entity* entity1 = new Entity();
    entity1->mesh = lee;
    entity1->id = 1;
    entity1->model.Translate(-1.5f, 0.0f, 0.0f);
    
    Entity* entity2 = new Entity();
    entity2->mesh = lee;
    entity2->id = 2;
    entity2->model.Translate(1.5f, 0.0f, 0.0f);
    
    Entity* entity3 = new Entity();
    entity3->mesh = lee;
    entity3->id = 3;
    entity3->model.Translate(0.0f, 0.0f, 0.0f);
    
    
    entities.push_back(entity1);
    entities.push_back(entity2);
    entities.push_back(entity3);

    std::cout << "Lee model loaded successfully!\n";
}


// Render one frame
void Application::Render(void)
{
    std::cout << "Rendering frame..." << std::endl; // Debug message

    framebuffer.Fill(Color(0, 0, 0)); // Clear framebuffer
    zBuffer.Fill(1.0f); // Initialize Z-buffer with maximum depth (1.0 means farthest)

    if (current_scene == 1) // Draw a single entity
    {
        if (!entities.empty() && entities[0]) // Ensure there's at least one entity
        {
            Entity* entity = entities[2];
            Color entityColor = Color(255, 255, 255); // Default color

            if (entity->id == 1) {
                entityColor = Color(0, 255, 0); // Green
            } else if (entity->id == 2) {
                entityColor = Color(0, 0, 255); // Blue
            }

            // FIX: Now pass `&zBuffer` instead of `entityColor`
            entity->Render(&framebuffer, camera, &zBuffer);
            std::cout << "Rendering single entity ID " << entity->id << " with color ("
                      << entityColor.r << ", " << entityColor.g << ", " << entityColor.b << ")" << std::endl;
        }
    }
    else if (current_scene == 2) // Draw multiple animated entities
    {
        for (Entity* entity : entities) {
            if (entity) {
                Color entityColor = Color(255, 255, 255); // Default color

                if (entity->id == 1) {
                    entityColor = Color(0, 255, 0); // Green
                } else if (entity->id == 2) {
                    entityColor = Color(0, 0, 255); // Blue
                }

                // FIX: Now pass `&zBuffer` instead of `entityColor`
                entity->Render(&framebuffer, camera, &zBuffer);
                std::cout << "Rendering entity ID " << entity->id << " with color ("
                          << entityColor.r << ", " << entityColor.g << ", " << entityColor.b << ")" << std::endl;
            }
        }
    }

    framebuffer.Render();
}
