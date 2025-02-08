void Image::ScanLineDDA(int x0, int y0, int x1, int y1,
    std::vector<std::pair<int, int>>& table, int minY) {
    // Early return if y0 == y1 (horizontal line)
    if (y0 == y1) return;

    // Ensure y0 < y1 for upward slope
    if (y0 > y1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    float slope = (float)(x1 - x0) / (y1 - y0); // Slope of the line
    float x = x0;

    for (int y = y0; y <= y1; ++y) {
        int row = y - minY; // Offset row based on minY
        if (row >= 0 && row < (int)table.size()) {
            table[row].first = std::min(table[row].first, (int)x);  // Update minX
            table[row].second = std::max(table[row].second, (int)x); // Update maxX
        }
        x += slope;
    }
}

void Image::DrawTriangle(const Vector2& p0, const Vector2& p1,
    const Vector2& p2, const Color& borderColor,
    bool isFilled, const Color& fillColor) {
    // Step 1: Sort vertices by Y-coordinate
    Vector2 v0 = p0, v1 = p1, v2 = p2;
    if (v0.y > v1.y) std::swap(v0, v1);
    if (v0.y > v2.y) std::swap(v0, v2);
    if (v1.y > v2.y) std::swap(v1, v2);

    // Step 2: Initialize AET
    int minY = (int)v0.y;
    int maxY = (int)v2.y;
    std::vector<std::pair<int, int>> AET(maxY - minY + 1, { INT_MAX, INT_MIN });

    // Step 3: Use ScanLineDDA to populate AET
    ScanLineDDA((int)v0.x, (int)v0.y, (int)v1.x, (int)v1.y, AET, minY);
    ScanLineDDA((int)v1.x, (int)v1.y, (int)v2.x, (int)v2.y, AET, minY);
    ScanLineDDA((int)v2.x, (int)v2.y, (int)v0.x, (int)v0.y, AET, minY);

    // Step 4: Fill the triangle
    if (isFilled) {
        for (int i = 0; i < (int)AET.size(); ++i) {
            if (AET[i].first <= AET[i].second) { // Valid row
                for (int x = AET[i].first; x <= AET[i].second; ++x) {
                    SetPixel(x, minY + i, fillColor);
                }
            }
        }
    }

    // Step 5: Draw the border
    DrawLineDDA((int)v0.x, (int)v0.y, (int)v1.x, (int)v1.y, borderColor);
    DrawLineDDA((int)v1.x, (int)v1.y, (int)v2.x, (int)v2.y, borderColor);
    DrawLineDDA((int)v2.x, (int)v2.y, (int)v0.x, (int)v0.y, borderColor);
}

void Image::DrawTriangleInterpolated(const Vector3& p0, const Vector3& p1, const Vector3& p2,
                                     const Color& c0, const Color& c1, const Color& c2) {
    // Step 1: Sort vertices by Y-coordinate
    Vector3 v0 = p0, v1 = p1, v2 = p2;
    Color col0 = c0, col1 = c1, col2 = c2;
    if (v0.y > v1.y) { std::swap(v0, v1); std::swap(col0, col1); }
    if (v0.y > v2.y) { std::swap(v0, v2); std::swap(col0, col2); }
    if (v1.y > v2.y) { std::swap(v1, v2); std::swap(col1, col2); }

    // Step 2: Initialize AET
    int minY = (int)v0.y;
    int maxY = (int)v2.y;
    std::vector<std::pair<int, int>> AET(maxY - minY + 1, { INT_MAX, INT_MIN });

    // Step 3: Populate AET using ScanLineDDA
    ScanLineDDA((int)v0.x, (int)v0.y, (int)v1.x, (int)v1.y, AET, minY);
    ScanLineDDA((int)v1.x, (int)v1.y, (int)v2.x, (int)v2.y, AET, minY);
    ScanLineDDA((int)v2.x, (int)v2.y, (int)v0.x, (int)v0.y, AET, minY);

    // Step 4: Compute barycentric coordinates & fill pixels
    for (int i = 0; i < (int)AET.size(); ++i) {
        if (AET[i].first <= AET[i].second) { // Valid row
            int y = minY + i;
            for (int x = AET[i].first; x <= AET[i].second; ++x) {
                // Compute Barycentric Coordinates
                float w0 = ((v1.y - v2.y) * (x - v2.x) + (v2.x - v1.x) * (y - v2.y)) /
                           ((v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y - v2.y));
                float w1 = ((v2.y - v0.y) * (x - v2.x) + (v0.x - v2.x) * (y - v2.y)) /
                           ((v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y - v2.y));
                float w2 = 1 - w0 - w1;

                // Ensure valid barycentric coordinates
                if (w0 < 0 || w1 < 0 || w2 < 0 || w0 > 1 || w1 > 1 || w2 > 1) continue;

                // Interpolate color
                Color pixelColor = col0 * w0 + col1 * w1 + col2 * w2;

                // Set pixel with interpolated color
                SetPixel(x, y, pixelColor);
            }
        }
    }
}
