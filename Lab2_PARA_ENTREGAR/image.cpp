void Image::DrawLineDDA(int x0, int y0, int x1, int y1,
    const Color& color) {
    // Calculate the differences in x and y
    int dx = x1 - x0;
    int dy = y1 - y0;

    // Determine the number of steps required
    int steps = std::max(abs(dx), abs(dy));

    // Compute the increment for each step
    float xIncrement = dx / static_cast<float>(steps);
    float yIncrement = dy / static_cast<float>(steps);

    // Starting point
    float x = x0;
    float y = y0;

    // Draw the line
    for (int i = 0; i <= steps; ++i) {
        // Set the pixel at the rounded position
        if (x >= 0 && x < width && y >= 0 && y < height) {
            SetPixel(static_cast<int>(round(x)),
                static_cast<int>(round(y)), color);
        }

        // Increment x and y
        x += xIncrement;
        y += yIncrement;
    }
}
//!!!!Añade la funció abans de aquesta linea
#ifndef IGNORE_LAMBDAS
