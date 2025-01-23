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

void Image::FlipX()
{
    int row_size = bytes_per_pixel * width;
    int half_width = width / 2; // Only iterate over half of each row

#pragma omp parallel for
    for (int y = 0; y < height; ++y)
    {
        Uint8* row_start = (Uint8*)pixels + y * row_size; // Pointer to the start of the current row
        Uint8* temp_pixel = new Uint8[bytes_per_pixel];   // Temporary storage for one pixel

        // Swap pixels in the row
        for (int x = 0; x < half_width; ++x)
        {
            Uint8* left_pixel = row_start + x * bytes_per_pixel;
            Uint8* right_pixel = row_start + (width - x - 1) * bytes_per_pixel;

            // Swap left and right pixels
            memcpy(temp_pixel, left_pixel, bytes_per_pixel);
            memcpy(left_pixel, right_pixel, bytes_per_pixel);
            memcpy(right_pixel, temp_pixel, bytes_per_pixel);
        }

        delete[] temp_pixel; // Free temporary pixel storage
    }
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
bool Image::LoadTGA(const char* filename, bool flip_y, bool flip_x)
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

void Image::DrawRect(int x, int y, int w, int h, const Color&
    borderColor, int borderWidth, bool isFilled,
    const Color& fillColor) {
    // Normalize coordinates to ensure positive width and height
    int startX = std::min(x, x + w); // Determine the left edge
    int startY = std::min(y, y + h); // Determine the top edge
    int endX = std::max(x, x + w);   // Determine the right edge
    int endY = std::max(y, y + h);   // Determine the bottom edge

    // Recalculate width and height
    int width = endX - startX;
    int height = endY - startY;

    // Fill the rectangle if required
    if (isFilled) {
        for (int i = startX + borderWidth; i < startX + width - borderWidth; ++i) {
            for (int j = startY + borderWidth; j < startY + height - borderWidth; ++j) {
                SetPixel(i, j, fillColor);
            }
        }
    }

    // Draw the top and bottom borders
    for (int i = 0; i < borderWidth; ++i) {
        // Top border
        for (int j = startX; j < startX + width; ++j) {
            SetPixel(j, startY + i, borderColor);
        }
        // Bottom border
        for (int j = startX; j < startX + width; ++j) {
            SetPixel(j, startY + height - 1 - i, borderColor);
        }
    }

    // Draw the left and right borders
    for (int i = 0; i < borderWidth; ++i) {
        // Left border
        for (int j = startY; j < startY + height; ++j) {
            SetPixel(startX + i, j, borderColor);
        }
        // Right border
        for (int j = startY; j < startY + height; ++j) {
            SetPixel(startX + width - 1 - i, j, borderColor);
        }
    }
}



#ifndef IGNORE_LAMBDAS


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



//void Image::DrawCircle(int xc, int yc, int radius, const Color& color, bool isFilled)

void Image::DrawCircle(int xc, int yc, int r, const Color&
    borderColor, int borderWidth, bool
    isFilled, const Color& fillColor) {
    // Start at the topmost point
    int x = 0;
    int y = r;
    int p = 1 - r; // Initial decision parameter

    // Helper function to draw symmetric points
    auto drawSymmetricPoints = [&](int x, int y) {
        SetPixel(xc + x, yc + y, borderColor);
        SetPixel(xc - x, yc + y, borderColor);
        SetPixel(xc + x, yc - y, borderColor);
        SetPixel(xc - x, yc - y, borderColor);
        SetPixel(xc + y, yc + x, borderColor);
        SetPixel(xc - y, yc + x, borderColor);
        SetPixel(xc + y, yc - x, borderColor);
        SetPixel(xc - y, yc - x, borderColor);
        };

    // Draw the initial set of symmetric points
    drawSymmetricPoints(x, y);

    // Iterate until x meets y
    while (x < y) {
        x++;
        if (p < 0) {
            // Midpoint is inside the circle
            p += 2 * x + 1;
        }
        else {
            // Midpoint is outside or on the circle
            y--;
            p += 2 * (x - y) + 1;
        }
        drawSymmetricPoints(x, y);
    }

    if (borderWidth != 1) {
        for (int i = 1; i <= borderWidth; i++) {
            DrawCircle(xc, yc, r - i, borderColor, 1, false, fillColor);
            DrawCircle(xc + 1, yc, r - i, borderColor, 1, false, fillColor);
            DrawCircle(xc, yc + 1, r - i, borderColor, 1, false, fillColor);
        }
    }


    if (isFilled == true) {
        for (int i = 0; i < r; i++) {
            DrawCircle(xc, yc, i, borderColor, 1);
            DrawCircle(xc + 1, yc, i, borderColor, 1);
            DrawCircle(xc, yc + 1, i, borderColor, 1);
        }
    }
}

void Image::DrawImage(const Image& image, int x, int y) {
    for (unsigned int i = 0; i < image.width; ++i) {
        for (unsigned int j = 0; j < image.height; ++j) {
            int targetX = x + i;
            int targetY = y + j;
            if (targetX >= 0 && targetX < width && targetY >= 0 && targetY < height) {
                SetPixel(targetX, targetY, image.GetPixel(i, j));
            }
        }
    }
}
