//Render per a Zbuffer
void Entity::Render(Image* framebuffer, Camera* camera, FloatImage* zBuffer) {
    if (!mesh || !camera || !zBuffer) return;

    const std::vector<Vector3>& vertices = mesh->GetVertices();
    
    for (size_t i = 0; i < vertices.size(); i += 3) { // Process each triangle
        Vector3 worldVertices[3];
        Vector3 screenVertices[3];
        float depthValues[3]; // Store Z values for interpolation
        Color vertexColors[3] = { Color(255, 0, 0), Color(0, 255, 0), Color(0, 0, 255) }; // RGB
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
            depthValues[j] = projected.z; // Store depth
        }

        if (!validTriangle) continue;

        // Call `DrawTriangleInterpolated` with the Z-buffer
        framebuffer->DrawTriangleInterpolated(
            screenVertices[0], screenVertices[1], screenVertices[2],
            vertexColors[0], vertexColors[1], vertexColors[2],
            zBuffer, depthValues
        );
    }
}


