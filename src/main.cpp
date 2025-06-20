#include "raylib.h"
#include "Body.hpp"
#include "raymath.h"
#include <vector>
#include <iostream>

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

	std::vector<Body> bodies;

	for (int i = 0; i < 1; ++i) {
		float x = static_cast<float>(GetRandomValue(0, SCREEN_WIDTH));
		float y = static_cast<float>(GetRandomValue(0, SCREEN_HEIGHT));
		float m = static_cast<float>(GetRandomValue(10, 1000));

		bodies.emplace_back(Vector2{ x, y }, m);  // mass = 100

		float v = 200.f;

		float velX = static_cast<float>(GetRandomValue(-v, v));
		float velY = static_cast<float>(GetRandomValue(-v, v));
	//	bodies[i].setVelocity({velX, velY});
	}

	Body* tempBody = nullptr;

	while (!WindowShouldClose()) {
		float panSpeed = 10.0f / camera.zoom;
		if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) camera.target.x += panSpeed;
		if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) camera.target.x -= panSpeed;
		if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) camera.target.y += panSpeed;
		if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W)) camera.target.y -= panSpeed;

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
				tempBody = new Body(mousePosWorld, 100.f);
			} else {
				tempBody->setVelocity(Vector2Scale(Vector2Subtract(mousePosWorld, tempBody->getPos()), -1.f));
				tempBody->setMass(tempBody->getMass() + wheel * 30);
			}
		} else if (tempBody != nullptr) {
			bodies.push_back(Body{tempBody->getPos(), tempBody->getMass()});
			bodies[bodies.size() - 1].setVelocity(tempBody->getVel());
			tempBody = nullptr;
		}

		BeginDrawing();
		BeginMode2D(camera);
		ClearBackground(BLACK);

		unsigned int substeps = 2;
		float dt = GetFrameTime() / substeps;

		for(int n = 0; n < substeps; n++) {
			for (Body& body : bodies) {
				body.beginUpdateVerlet(dt);
			}

			for (size_t i = 0; i < bodies.size(); ++i) {
				for (size_t j = i + 1; j < bodies.size(); ++j) {
					Body::attract(bodies[i], bodies[j]);
				}
			}

			for (Body& body : bodies) {
				body.endUpdateVerlet(dt);
				if((frame * substeps + n) % 10 == 0) body.addPath();
			}

			for (size_t i = 0; i < bodies.size(); ++i) {
				for (size_t j = i + 1; j < bodies.size(); ++j) {
					Body::collide(bodies[i], bodies[j]);
				}
			}
		}

		for(Body& body: bodies) {
			body.renderPath();
		}

		for(Body& body : bodies) {
			body.render();
		}

		if(tempBody != nullptr) {
			tempBody-> render();
		}


		EndMode2D();
		EndDrawing();
		frame++;
	}

	CloseWindow();

	return 0;
}
