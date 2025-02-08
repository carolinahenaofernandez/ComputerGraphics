void Application::Init(void)
{
    std::cout << "Initiating app..." << std::endl;
    camera = new Camera();
    camera->LookAt(Vector3(0,0,1), Vector3(0,0,0), Vector3(0,-1,0));
    camera->SetPerspective(45.0f, (float)framebuffer.width / (float)framebuffer.height, 0.1f, 50.0f);

    Mesh* lee = new Mesh();
    if (!lee->LoadOBJ("meshes/lee.obj")) {
        std::cerr << "Failed to load lee.obj!\n";
        return;
    }

    Entity* leeEntity = new Entity();
    leeEntity->mesh = lee;

    leeEntity->model = Matrix44();

    entities.push_back(leeEntity);

    std::cout << "Lee model loaded successfully!\n";
}


// Render one frame
void Application::Render(void)
{
    //...
    std::cout << "Rendering frame..." << std::endl; // Debug message
    for (Entity* entity : entities){
        if (entity){
            entity->Render(&framebuffer, camera, Color(255,255,255)); // White wireframe
            std::cout << "Rendering entity..." << std::endl;
        }
    }
    framebuffer.Render();
}
