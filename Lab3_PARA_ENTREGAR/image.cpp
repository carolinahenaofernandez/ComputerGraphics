#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "GL/glew.h"
#include "../extra/picopng.h"
#include "image.h"
#include "utils.h"
#include "camera.h"
#include "mesh.h"




Image::Image() {
    width = 0; height = 0;
    pixels = NULL;
}

Image::Image(unsigned int width, unsigned int height)
{
    this->width = width;
    this->height = height;
    pixels = new Color[width * height];
    memset(pixels, 0, width * height * sizeof(Color));
}

// Copy constructor
Image::Image(const Image& c)
{
    pixels = NULL;
    width = c.width;
    height = c.height;
    bytes_per_pixel = c.bytes_per_pixel;
    if (c.pixels)
    {
        pixels = new Color[width * height];
        memcpy(pixels, c.pixels, width * height * bytes_per_pixel);
    }
}

// Assign operator
Image& Image::operator = (const Image& c)
{
    if (pixels) delete pixels;
    pixels = NULL;

    width = c.width;
    height = c.height;
    bytes_per_pixel = c.bytes_per_pixel;

    if (c.pixels)
    {
        pixels = new Color[width * height * bytes_per_pixel];
        memcpy(pixels, c.pixels, width * height * bytes_per_pixel);
    }
    return *this;
}

Image::~Image()
{
    if (pixels)
        delete pixels;
}

void Image::Render()
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glDrawPixels(width, height, bytes_per_pixel == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

// Change image size (the old one will remain in the top-left corner)
void Image::Resize(unsigned int width, unsigned int height)
{
    Color* new_pixels = new Color[width * height];
    unsigned int min_width = this->width > width ? width : this->width;
    unsigned int min_height = this->height > height ? height : this->height;

    for (unsigned int x = 0; x < min_width; ++x)
        for (unsigned int y = 0; y < min_height; ++y)
            new_pixels[y * width + x] = GetPixel(x, y);

    delete pixels;
    this->width = width;
    this->height = height;
    pixels = new_pixels;
}

// Change image size and scale the content
void Image::Scale(unsigned int width, unsigned int height)
{
    Color* new_pixels = new Color[width * height];

    for (unsigned int x = 0; x < width; ++x)
        for (unsigned int y = 0; y < height; ++y)
            new_pixels[y * width + x] = GetPixel((unsigned int)(this->width * (x / (float)width)), (unsigned int)(this->height * (y / (float)height)));

    delete pixels;
    this->width = width;
    this->height = height;
    pixels = new_pixels;
}

Image Image::GetArea(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height)
{
    Image result(width, height);
    for (unsigned int x = 0; x < width; ++x)
        for (unsigned int y = 0; y < height; ++y)
        {
            if ((x + start_x) < this->width && (y + start_y) < this->height)
                result.SetPixelUnsafe(x, y, GetPixel(x + start_x, y + start_y));
        }
    return result;
}

void Image::FlipY()
{
    int row_size = bytes_per_pixel * width;
    Uint8* temp_row = new Uint8[row_size];
#pragma omp simd
    for (int y = 0; y < height * 0.5; y += 1)
    {
        Uint8* pos = (Uint8*)pixels + y * row_size;
        memcpy(temp_row, pos, row_size);
        Uint8* pos2 = (Uint8*)pixels + (height - y - 1) * row_size;
        memcpy(pos, pos2, row_size);
        memcpy(pos2, temp_row, row_size);
    }
    delete[] temp_row;
}

bool Image::LoadPNG(const char* filename, bool flip_y)
{
    std::string sfullPath = absResPath(filename);
    std::ifstream file(sfullPath, std::ios::in | std::ios::binary | std::ios::ate);

    // Get filesize
    std::streamsize size = 0;
    if (file.seekg(0, std::ios::end).good()) size = file.tellg();
    if (file.seekg(0, std::ios::beg).good()) size -= file.tellg();

    if (!size)
        return false;

    std::vector<unsigned char> buffer;

    // Read contents of the file into the vector
    if (size > 0)
    {
        buffer.resize((size_t)size);
        file.read((char*)(&buffer[0]), size);
    }
    else
        buffer.clear();

    std::vector<unsigned char> out_image;

    if (decodePNG(out_image, width, height, buffer.empty() ? 0 : &buffer[0], (unsigned long)buffer.size(), true) != 0)
        return false;

    size_t bufferSize = out_image.size();
    unsigned int originalBytesPerPixel = (unsigned int)bufferSize / (width * height);

    // Force 3 channels
    bytes_per_pixel = 3;

    if (originalBytesPerPixel == 3) {
        pixels = new Color[bufferSize];
        memcpy(pixels, &out_image[0], bufferSize);
    }
    else if (originalBytesPerPixel == 4) {

        unsigned int newBufferSize = width * height * bytes_per_pixel;
        pixels = new Color[newBufferSize];

        unsigned int k = 0;
        for (unsigned int i = 0; i < bufferSize; i += originalBytesPerPixel) {
            pixels[k] = Color(out_image[i], out_image[i + 1], out_image[i + 2]);
            k++;
        }
    }

    // Flip pixels in Y
    if (flip_y)
        FlipY();

    return true;
}

// Loads an image from a TGA file
bool Image::LoadTGA(const char* filename, bool flip_y)
{
    unsigned char TGAheader[12] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    unsigned char TGAcompare[12];
    unsigned char header[6];
    unsigned int imageSize;
    unsigned int bytesPerPixel;

    std::string sfullPath = absResPath(filename);

    FILE* file = fopen(sfullPath.c_str(), "rb");
    if (file == NULL || fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare) ||
        memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0 ||
        fread(header, 1, sizeof(header), file) != sizeof(header))
    {
        std::cerr << "File not found: " << sfullPath.c_str() << std::endl;
        if (file == NULL)
            return NULL;
        else
        {
            fclose(file);
            return NULL;
        }
    }

    TGAInfo* tgainfo = new TGAInfo;

    tgainfo->width = header[1] * 256 + header[0];
    tgainfo->height = header[3] * 256 + header[2];

    if (tgainfo->width <= 0 || tgainfo->height <= 0 || (header[4] != 24 && header[4] != 32))
    {
        fclose(file);
        delete tgainfo;
        return NULL;
    }

    tgainfo->bpp = header[4];
    bytesPerPixel = tgainfo->bpp / 8;
    imageSize = tgainfo->width * tgainfo->height * bytesPerPixel;

    tgainfo->data = new unsigned char[imageSize];

    if (tgainfo->data == NULL || fread(tgainfo->data, 1, imageSize, file) != imageSize)
    {
        if (tgainfo->data != NULL)
            delete tgainfo->data;

        fclose(file);
        delete tgainfo;
        return false;
    }

    fclose(file);

    // Save info in image
    if (pixels)
        delete pixels;

    width = tgainfo->width;
    height = tgainfo->height;
    pixels = new Color[width * height];

    // Convert to float all pixels
    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            unsigned int pos = y * width * bytesPerPixel + x * bytesPerPixel;
            // Make sure we don't access out of memory
            if ((pos < imageSize) && (pos + 1 < imageSize) && (pos + 2 < imageSize))
                SetPixelUnsafe(x, height - y - 1, Color(tgainfo->data[pos + 2], tgainfo->data[pos + 1], tgainfo->data[pos]));
        }
    }

    // Flip pixels in Y
    if (flip_y)
        FlipY();

    delete tgainfo->data;
    delete tgainfo;

    return true;
}

// Saves the image to a TGA file
bool Image::SaveTGA(const char* filename)
{
    unsigned char TGAheader[12] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    std::string fullPath = absResPath(filename);
    FILE* file = fopen(fullPath.c_str(), "wb");
    if (file == NULL)
    {
        perror("Failed to open file: ");
        return false;
    }

    unsigned short header_short[3];
    header_short[0] = width;
    header_short[1] = height;
    unsigned char* header = (unsigned char*)header_short;
    header[4] = 24;
    header[5] = 0;

    fwrite(TGAheader, 1, sizeof(TGAheader), file);
    fwrite(header, 1, 6, file);

    // Convert pixels to unsigned char
    unsigned char* bytes = new unsigned char[width * height * 3];
    for (unsigned int y = 0; y < height; ++y)
        for (unsigned int x = 0; x < width; ++x)
        {
            Color c = pixels[y * width + x];
            unsigned int pos = (y * width + x) * 3;
            bytes[pos + 2] = c.r;
            bytes[pos + 1] = c.g;
            bytes[pos] = c.b;
        }

    fwrite(bytes, 1, width * height * 3, file);
    fclose(file);

    return true;
}

void Image::DrawRect(int x, int y, int w, int h, const Color& c)
{

    for (int i = 0; i < w; ++i) {
        SetPixelUnsafe(x + i, y, c);
        SetPixelUnsafe(x + i, y + h - 1, c);
    }

    for (int j = 0; j < h; ++j) {
        SetPixelUnsafe(x, y + j, c);
        SetPixelUnsafe(x + w - 1, y + j, c);
    }
}

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

// This function draws a triangle with barycentric interpolation for both color and texture
void Image::DrawTriangleInterpolated(const sTriangleInfo& triangle, FloatImage* zBuffer, bool occlusions) {
    // Extract triangle data (vertices, UVs, colors, and texture)
    const Vector3& p0 = triangle.p0;
    const Vector3& p1 = triangle.p1;
    const Vector3& p2 = triangle.p2;
    const Vector2& uv0 = triangle.uv0;
    const Vector2& uv1 = triangle.uv1;
    const Vector2& uv2 = triangle.uv2;
    const Color& c0 = triangle.c0;
    const Color& c1 = triangle.c1;
    const Color& c2 = triangle.c2;
    Image* texture = triangle.texture;

    // Step 1: Find the vertical bounds of the triangle by sorting its vertices based on the Y-coordinate
    int minY = std::min({ (int)p0.y, (int)p1.y, (int)p2.y });
    int maxY = std::max({ (int)p0.y, (int)p1.y, (int)p2.y });

    // Step 2: Create a table to store the horizontal bounds (min and max X) for each row (Y)
    std::vector<std::pair<int, int>> table(maxY - minY + 1, { INT_MAX, INT_MIN });

    // Step 3: Use ScanLineDDA to populate the table with the horizontal bounds for each row
    ScanLineDDA((int)p0.x, (int)p0.y, (int)p1.x, (int)p1.y, table, minY);
    ScanLineDDA((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, table, minY);
    ScanLineDDA((int)p2.x, (int)p2.y, (int)p0.x, (int)p0.y, table, minY);

    // Step 4: Iterate through the filled area (rows between minY and maxY) and draw the pixels
    for (int i = 0; i < (int)table.size(); ++i) {
        if (table[i].first <= table[i].second) {  // If the row has valid horizontal bounds
            int y = minY + i;  // Current Y-coordinate

            // Loop through each pixel in the row (between the left and right bounds)
            for (int x = table[i].first; x <= table[i].second; ++x) {
                Vector3 p(x, y, 0.0f);  // Current pixel position

                // Step 5: Compute barycentric coordinates for the pixel (u, v, w)
                Vector3 v0 = p1 - p0;
                Vector3 v1 = p2 - p0;
                Vector3 vp = p - p0;
                Vector3 v2 = p1 - p2;
                Vector3 vp2 = p - p1;

                // Compute areas for barycentric coordinates
                float area = (v0.Cross(v1).Length()) / 2.0f;
                float area0 = (v2.Cross(vp2).Length()) / 2.0f;
                float area1 = (vp.Cross(v1).Length()) / 2.0f;
                float area2 = (v0.Cross(vp).Length()) / 2.0f;

                // Barycentric coordinates
                float u = area0 / area;
                float v = area1 / area;
                float w = area2 / area;

                // Normalize barycentric coordinates to ensure they sum to 1
                float sum = u + v + w;
                u /= sum;
                v /= sum;
                w /= sum;

                // Step 6: Compute the Z-depth for the pixel using barycentric interpolation
                float z = p0.z * u + p1.z * v + p2.z * w;

                // Step 7: Check if the pixel is inside the triangle (u, v, w >= 0) and if it should be drawn
                if (u >= 0 && v >= 0 && w >= 0) {
                    if (occlusions) {  // If occlusions (Z-buffer) are enabled
                        // Check if the pixel should be drawn based on the Z-buffer depth
                        float currentDepth = zBuffer->GetPixelSafe(x, y);
                        if (z < currentDepth) {  // If the pixel is closer than the current Z-buffer value
                            Color color;

                            // Choose whether to use texture or vertex color
                            if (texture == nullptr) {
                                // Use interpolated vertex colors if no texture is provided
                                color = c0 * u + c1 * v + c2 * w;
                            }
                            else {
                                // Use texture mapping if a texture is available
                                float texU = uv0.x * u + uv1.x * v + uv2.x * w;
                                float texV = uv0.y * u + uv1.y * v + uv2.y * w;
                                int texX = static_cast<int>(texU * (texture->width - 1));
                                int texY = static_cast<int>(texV * (texture->height - 1));
                                color = texture->GetPixelSafe(texX, texY);  // Fetch the pixel color from the texture
                            }

                            SetPixel(x, y, color);  // Set the pixel color
                            zBuffer->SetPixel(x, y, z);  // Update the Z-buffer with the new depth value
                        }
                    }
                    else {  // If occlusions are not enabled (no Z-buffer)
                        Color color;

                        // Choose whether to use texture or vertex color
                        if (texture == nullptr) {
                            // Use interpolated vertex colors if no texture is applied
                            color = c0 * u + c1 * v + c2 * w;
                        }
                        else {
                            // Use texture mapping if a texture is applied
                            float texU = uv0.x * u + uv1.x * v + uv2.x * w;
                            float texV = uv0.y * u + uv1.y * v + uv2.y * w;
                            int texX = static_cast<int>(texU * (texture->width - 1));
                            int texY = static_cast<int>(texV * (texture->height - 1));
                            color = texture->GetPixelSafe(texX, texY);  // Fetch the pixel color from the texture
                        }

                        SetPixel(x, y, color);  // Set the pixel color
                    }
                }
            }
        }
    }
}



#ifndef IGNORE_LAMBDAS

// You can apply and algorithm for two images and store the result in the first one
// ForEachPixel( img, img2, [](Color a, Color b) { return a + b; } );
template <typename F>
void ForEachPixel(Image& img, const Image& img2, F f) {
    for (unsigned int pos = 0; pos < img.width * img.height; ++pos)
        img.pixels[pos] = f(img.pixels[pos], img2.pixels[pos]);
}

#endif

FloatImage::FloatImage(unsigned int width, unsigned int height)
{
    this->width = width;
    this->height = height;
    pixels = new float[width * height];
    memset(pixels, 0, width * height * sizeof(float));
}

// Copy constructor
FloatImage::FloatImage(const FloatImage& c) {
    pixels = NULL;

    width = c.width;
    height = c.height;
    if (c.pixels)
    {
        pixels = new float[width * height];
        memcpy(pixels, c.pixels, width * height * sizeof(float));
    }
}

// Assign operator
FloatImage& FloatImage::operator = (const FloatImage& c)
{
    if (pixels) delete pixels;
    pixels = NULL;

    width = c.width;
    height = c.height;
    if (c.pixels)
    {
        pixels = new float[width * height * sizeof(float)];
        memcpy(pixels, c.pixels, width * height * sizeof(float));
    }
    return *this;
}

FloatImage::~FloatImage()
{
    if (pixels)
        delete pixels;
}

// Change image size (the old one will remain in the top-left corner)
void FloatImage::Resize(unsigned int width, unsigned int height)
{
    float* new_pixels = new float[width * height];
    unsigned int min_width = this->width > width ? width : this->width;
    unsigned int min_height = this->height > height ? height : this->height;

    for (unsigned int x = 0; x < min_width; ++x)
        for (unsigned int y = 0; y < min_height; ++y)
            new_pixels[y * width + x] = GetPixel(x, y);

    delete pixels;
    this->width = width;
    this->height = height;
    pixels = new_pixels;
}

