#include "Body.hpp"
#include "World.hpp"
#include <cmath>
#include <raylib.h>
#include <raymath.h>
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

#define SCREEN_WIDTH (1600)
#define SCREEN_HEIGHT (900)

#define WINDOW_TITLE "Orbits!"

#ifdef __EMSCRIPTEN__
EM_BOOL on_resize(int eventType, const EmscriptenUiEvent *uiEvent, void *userData) {
    int width = (int)uiEvent->windowInnerWidth;
    int height = (int)uiEvent->windowInnerHeight;
    SetWindowSize(width, height); // Tells Raylib to update its internal screen size
    return EM_TRUE;
}
#endif

int main(void) {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);

	#ifdef __EMSCRIPTEN__
	double width, height;
    emscripten_get_element_css_size("#canvas", &width, &height);
    SetWindowSize((int)width, (int)height);
	emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, on_resize);
	#endif

	SetTargetFPS(60);
	unsigned int frame = 0;
	bool paused = false;

	Camera2D camera = {0};
	camera.zoom = 1.0f;
	float zoomSpeed = 0.2f;

	World world;
	// world.addRandomBodies(1, SCREEN_WIDTH, SCREEN_HEIGHT);

	Body *tempBody = nullptr;

	while (!WindowShouldClose()) {
		// ----------------INPUT HANDLING----------------------
		Vector2 mousePosScreen = GetMousePosition();
		Vector2 mousePosWorld = GetScreenToWorld2D(mousePosScreen, camera);
		float wheel = GetMouseWheelMove();

		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			Vector2 delta = GetMouseDelta();
			delta /= -camera.zoom;
			camera.target += delta;
		}

		if (wheel != 0 && tempBody == nullptr) {
			// get the world point that is under the mouse
			Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
			// set the offset to where the mouse is
			camera.offset = GetMousePosition();
			// set the target to match, so that the camera maps the world space point
			// under the cursor to the screen space point under the cursor at any zoom
			camera.target = mouseWorldPos;

			camera.zoom += wheel * 0.125f;
			camera.zoom = fmax(camera.zoom, 0.125f);
		}

		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			if (tempBody == nullptr) {
				// if mouse is down AND temp body doesnt exist, create it.
				tempBody = new Body(mousePosWorld, Vector2Zero(), 100.f);
			} else {
				// if mouse is down AND temp body already exists, update it.
				tempBody->setVelocity((mousePosWorld - tempBody->getPos()) * -1);
				tempBody->setMass(tempBody->getMass() + wheel * 30);
			}
		} else if (tempBody != nullptr) {
			// if mouse button is up BUT temp body exists (user lifted the mouse), add
			// it to the world.
			world.addBody(tempBody->getPos(), tempBody->getVel(),
			              tempBody->getMass());
			delete tempBody;
			tempBody = nullptr;
		}

		if (IsKeyPressed(KEY_SPACE))
			paused = !paused;

		// --------------------UPDATE BODIES-------------------

		if (!paused)
			world.update(GetFrameTime(), 4);
		if (frame % 4 == 0)
			world.addPath();

		// --------------------RENDER STUFF--------------------

		BeginDrawing();
		BeginMode2D(camera);
		ClearBackground(Color{23, 22, 31, 255});

		world.render();

		// ---------------HANDLE TEMPORARY BODY-----------------
		if (tempBody != nullptr) {
			World worldCopy = world.copy();
			worldCopy.addBody(tempBody->getPos(), tempBody->getVel(),
			                  tempBody->getMass());
			for (int i = 0; i < 100; i++) {
				worldCopy.update(0.1, 1);
				worldCopy.addPath(true);
			}
			tempBody->render();
			worldCopy.renderLastBodyPath();
		}

		EndMode2D();

		if (paused) {
			DrawText("Paused", SCREEN_WIDTH - 100, 30, 24, RED);
		}

		EndDrawing();
		frame++;
	}

	CloseWindow();

	return 0;
}
