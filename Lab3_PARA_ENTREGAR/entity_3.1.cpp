//Canvia a aquest render pook <3
void Entity::Render(Image* framebuffer, Camera* camera, const Color& c) {
    if (!mesh || !camera) return;

    const std::vector<Vector3>& vertices = mesh->GetVertices();
    
    for (size_t i = 0; i < vertices.size(); i += 3) { // Process each triangle
        Vector3 worldVertices[3];
        Vector2 screenVertices[3]; // Store 2D screen-space coordinates
        bool validTriangle = true;

        for (int j = 0; j < 3; ++j) {
            worldVertices[j] = model * vertices[i + j];
            bool negZ = false;
            Vector3 projected = camera->ProjectVector(worldVertices[j], negZ);

            // Clip-space check
            if (projected.x < -1 || projected.x > 1 ||
                projected.y < -1 || projected.y > 1 ||
                projected.z < -1 || projected.z > 1) {
                validTriangle = false;
            }

            // Convert to screen space
            screenVertices[j].x = (projected.x + 1.0f) * 0.5f * framebuffer->width;
            screenVertices[j].y = (1.0f - projected.y) * 0.5f * framebuffer->height; // Flip y-axis.
        }

        if (!validTriangle) continue;

        // Call your existing `DrawTriangle` function
        framebuffer->DrawTriangle(
            screenVertices[0],
            screenVertices[1],
            screenVertices[2],
            c,   // Border color
            true, // Fill enabled
            c    // Fill color (same as border for now)
        );
    }
}
