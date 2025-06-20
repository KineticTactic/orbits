#include "raylib.h"
#include "Body.hpp"
#include <vector>

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

	std::vector<Body> bodies;

	for (int i = 0; i < 20; ++i) {
		float x = static_cast<float>(GetRandomValue(0, SCREEN_WIDTH));
		float y = static_cast<float>(GetRandomValue(0, SCREEN_HEIGHT));
		float m = static_cast<float>(GetRandomValue(10, 1000));

		bodies.emplace_back(Vector2{ x, y }, m);  // mass = 100

		float v = 200.f;

		float velX = static_cast<float>(GetRandomValue(-v, v));
		float velY = static_cast<float>(GetRandomValue(-v, v));
		bodies[i].setVelocity({velX, velY});
	}

	while (!WindowShouldClose()) {
		float panSpeed = 10.0f / camera.zoom;  // consistent speed regardless of zoom
		if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) camera.target.x += panSpeed;
		if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) camera.target.x -= panSpeed;
		if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) camera.target.y += panSpeed;
		if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W)) camera.target.y -= panSpeed;

		// Zoom camera with mouse wheel (log scale)
		float wheel = GetMouseWheelMove();
		if (wheel != 0) {
			float zoomFactor = 1.1f;
			camera.zoom *= (wheel > 0) ? zoomFactor : (1.0f / zoomFactor);
		}

		BeginDrawing();
		BeginMode2D(camera);
		ClearBackground(BLACK);

		unsigned int substeps = 2;
		float dt = GetFrameTime() / substeps;

		for(int n = 0; n < substeps; n++) {
			for (Body& body : bodies) {
				body.beginUpdate(dt);
			}

			for (size_t i = 0; i < bodies.size(); ++i) {
				for (size_t j = i + 1; j < bodies.size(); ++j) {
					Body::attract(bodies[i], bodies[j]);
				}
			}

			for (Body& body : bodies) {
				// body.springForce();  // Uncomment if needed
				body.updateImplicitEuler(dt);  // Or updateVelocityVerlet(dt) if using that
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


		EndMode2D();
		EndDrawing();
		frame++;
	}

	CloseWindow();

	return 0;
}
