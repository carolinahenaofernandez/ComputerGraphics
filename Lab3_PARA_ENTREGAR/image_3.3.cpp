//Triangle interpolated per a zbuffer
void Image::DrawTriangleInterpolated(const Vector3& p0, const Vector3& p1, const Vector3& p2,
                                     const Color& c0, const Color& c1, const Color& c2,
                                     FloatImage* zBuffer, const float depthValues[3]) {
    // Step 1: Sort vertices by Y-coordinate
    Vector3 v0 = p0, v1 = p1, v2 = p2;
    Color col0 = c0, col1 = c1, col2 = c2;
    float z0 = depthValues[0], z1 = depthValues[1], z2 = depthValues[2];

    if (v0.y > v1.y) { std::swap(v0, v1); std::swap(col0, col1); std::swap(z0, z1); }
    if (v0.y > v2.y) { std::swap(v0, v2); std::swap(col0, col2); std::swap(z0, z2); }
    if (v1.y > v2.y) { std::swap(v1, v2); std::swap(col1, col2); std::swap(z1, z2); }

    // Step 2: Initialize AET
    int minY = (int)v0.y;
    int maxY = (int)v2.y;
    std::vector<std::pair<int, int>> AET(maxY - minY + 1, { INT_MAX, INT_MIN });

    // Step 3: Populate AET using ScanLineDDA
    ScanLineDDA((int)v0.x, (int)v0.y, (int)v1.x, (int)v1.y, AET, minY);
    ScanLineDDA((int)v1.x, (int)v1.y, (int)v2.x, (int)v2.y, AET, minY);
    ScanLineDDA((int)v2.x, (int)v2.y, (int)v0.x, (int)v0.y, AET, minY);

    // Step 4: Compute barycentric coordinates & fill pixels with Z-Buffer check
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

                // Interpolate depth
                float depth = z0 * w0 + z1 * w1 + z2 * w2;

                // Depth Test (Z-Buffer)
                if (depth < zBuffer->GetPixel(x, y)) {
                    zBuffer->SetPixel(x, y, depth); // Update Z-Buffer
                    Color pixelColor = col0 * w0 + col1 * w1 + col2 * w2;
                    SetPixel(x, y, pixelColor);
                }
            }
        }
    }
}
