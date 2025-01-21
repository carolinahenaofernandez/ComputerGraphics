/*
	+ This class encapsulates the application, is in charge of creating the data, getting the user input, process the update and render.
*/

#pragma once

#include "main/includes.h"
#include "framework.h"
#include "image.h"

class Application
{
public:


	// Window

	SDL_Window* window = nullptr;
	int window_width;
	int window_height;

	float time;
	int exercise = 0;
	int startX, startY;   // Starting mouse position
	int endX, endY;       // Ending mouse position
	bool shouldRender;    // Flag to indicate if something should be rendered
	int ComputeRadius(int x1, int y1, int x2, int y2);
	Color selected_color = Color(255, 255, 255); // Default to white

	// Add p1 and p2 as pointers to Vector2
	Vector2* p1 = NULL; // Pointer to the first point
	Vector2* p2 = NULL; // Pointer to the second point

	// Input
	const Uint8* keystate;
	int mouse_state; // Tells which buttons are pressed
	Vector2 mouse_position; // Last mouse position
	Vector2 mouse_delta; // Mouse movement in the last frame

	void OnKeyPressed(SDL_KeyboardEvent event);
	void OnMouseButtonDown(SDL_MouseButtonEvent event);
	void OnMouseButtonUp(SDL_MouseButtonEvent event);
	void OnMouseMove(SDL_MouseButtonEvent event);
	void OnWheel(SDL_MouseWheelEvent event);
	void OnFileChanged(const char* filename);

	// CPU Global framebuffer
	Image framebuffer;

	// Constructor and main methods
	Application(const char* caption, int width, int height);
	~Application();

	void Init(void);
	void Render(void);
	void Update(float dt);

	// Other methods to control the app
	void SetWindowSize(int width, int height) {
		glViewport(0, 0, width, height);
		this->window_width = width;
		this->window_height = height;
		this->framebuffer.Resize(width, height);
	}

	Vector2 GetWindowSize()
	{
		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		return Vector2(float(w), float(h));
	}
};
