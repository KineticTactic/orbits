#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include "World.hpp"
#include "Body.hpp"

#define SCREEN_WIDTH (1600)
#define SCREEN_HEIGHT (900)

#define WINDOW_TITLE "Orbits!"

int main(void)
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
	SetTargetFPS(60);
	unsigned int frame = 0;

	Camera2D camera = { 0 };
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;
	float zoomSpeed = 0.2f;

	World world;
	//world.addRandomBodies(1, SCREEN_WIDTH, SCREEN_HEIGHT);

	Body* tempBody = nullptr;

	while (!WindowShouldClose()) {
		Vector2 mousePosScreen = GetMousePosition();
		Vector2 mousePosWorld = GetScreenToWorld2D(mousePosScreen, camera);

		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			Vector2 delta = GetMouseDelta();
			delta = Vector2Scale(delta, -1.0f / camera.zoom);
			camera.target = Vector2Add(camera.target, delta);
		}

		float wheel = GetMouseWheelMove();
		if (wheel != 0 && tempBody == nullptr) {
			// get the world point that is under the mouse
			Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
			
			// set the offset to where the mouse is
			camera.offset = GetMousePosition();

			// set the target to match, so that the camera maps the world space point under the cursor to the screen space point under the cursor at any zoom
			camera.target = mouseWorldPos;

			// zoom
			camera.zoom += wheel * 0.125f;
			if (camera.zoom < 0.125f)
				camera.zoom = 0.125f;
		}

		if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			if (tempBody == nullptr) {
				tempBody = new Body(mousePosWorld, Vector2Zero(), 100.f);
			} else {
				tempBody->setVelocity(Vector2Scale(Vector2Subtract(mousePosWorld, tempBody->getPos()), -1.f));
				tempBody->setMass(tempBody->getMass() + wheel * 30);
			}
		} else if (tempBody != nullptr) {
			world.addBody(tempBody->getPos(), tempBody->getVel(), tempBody->getMass());
			tempBody = nullptr;
		}

		BeginDrawing();
		BeginMode2D(camera);
		ClearBackground(BLACK);

		world.update(GetFrameTime(), 4);
		world.render();

		if(tempBody != nullptr) {
			World worldCopy = world.copy();
			worldCopy.addBody(tempBody->getPos(), tempBody->getVel(), tempBody->getMass());
			for(int i = 0; i < 100; i++) {
				worldCopy.update(0.1, 1);
			}
			tempBody-> render();
			worldCopy.renderLastBodyPath();
		}


		EndMode2D();
		EndDrawing();
		frame++;
	}

	CloseWindow();

	return 0;
}
