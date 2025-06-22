#include "App.hpp"
#include <raylib.h>
#include <raymath.h>

App::App()
	: world(),
	  tempWorld(nullptr),
	  tempBody(nullptr),
	  gui(),
	  frame(0),
	  zoomSpeed(0.2f) {
	camera = {0};
	camera.zoom = 1.f;

	//world.addBody({0.f, 0.f}, {10.f, 10.f}, 100);
}

void App::handleInput() {
	if(GetTouchPointCount() > 0) {
		int tCount = GetTouchPointCount();
		const int maxTouchPointCount = 10;
		if(tCount > maxTouchPointCount) tCount = maxTouchPointCount;

		std::map<int, Vector2> newTouchPoints;
		for(int i = 0; i < tCount; i++) {
			int touchPointId = GetTouchPointId(i);
			Vector2 touchPosition = GetTouchPosition(i);
			if (gui.isOverlapping(touchPosition)) continue;
			newTouchPoints[touchPointId] = touchPosition;
		}

		std::vector<int> touchPointIDs;

		for (auto it = newTouchPoints.begin(); it != newTouchPoints.end(); it++) {
			int touchPointId = it->first;
			if(touchPoints.find(touchPointId) != touchPoints.end()) {
				touchPointIDs.push_back(touchPointId);
			}
		}

		switch (gui.getSelected()) {
		case 0:
			handleTouchInputCamera(touchPoints, newTouchPoints, touchPointIDs);
			break;
		case 1:
			handleTouchInputAddBody(touchPoints, newTouchPoints, touchPointIDs);
			break;
		}
		touchPoints = newTouchPoints;
		return;
	}

	touchPoints.clear();

	Vector2 mousePosScreen = GetMousePosition();
	Vector2 mousePosWorld = GetScreenToWorld2D(mousePosScreen, camera);
	float wheel = GetMouseWheelMove();

	if (IsKeyPressed(KEY_SPACE))
		gui.togglePaused();

	if(gui.isMouseOverGUI()) return;

	// ZOOMING IN AND OUT
	if (wheel != 0 && tempBody == nullptr) {
		camera.offset = mousePosScreen;
		camera.target = mousePosWorld;
		camera.zoom += wheel * 0.125f;
		camera.zoom = Clamp(camera.zoom, 0.1f, 20.f);
	}

	// PAN THE VIEW
	if ((gui.getSelected() == PAN && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) ||
	        IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
		Vector2 delta = GetMouseDelta();
		delta /= -camera.zoom;
		camera.target += delta;
	}

	// ADD and DRAG new bodies
	if (gui.getSelected() == ADD && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
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
		world.addBody(tempBody->getPos(), tempBody->getVel(), tempBody->getMass());
		delete tempBody;
		delete tempWorld;
		tempBody = nullptr;
		tempWorld = nullptr;
	}
}

void App::update() {
	if(!gui.getPaused())
		world.update(GetFrameTime(), 4);

	if (!gui.getPaused() && frame % 4 == 0)
		world.addPath();

	if (tempBody != nullptr) {
		tempWorld = world.copy();
		tempWorld->addBody(tempBody->getPos(), tempBody->getVel(),
		                   tempBody->getMass());
		for (int i = 0; i < 100; i++) {
			tempWorld->update(0.1, 1);
			tempWorld->addPath(true);
		}
	}

	if(IsWindowResized()) gui.handleResize();

	frame++;
}

void App::render() {
	BeginMode2D(camera);
	world.render();

	if(tempWorld != nullptr && tempBody != nullptr) {
		tempWorld->renderLastBodyPath();
		tempBody->render();
	}
	EndMode2D();

	gui.render();

	if (gui.getPaused()) {
//		DrawText("Paused", GetScreenWidth() - 100, font=, 24, RED);
	}
}

void App::handleTouchInputCamera(std::map<int, Vector2>& touchPoints, std::map<int, Vector2>& newTouchPoints, std::vector<int>& touchPointIDs) {
	if(touchPointIDs.size() == 1) {
		Vector2 touchOld = touchPoints[touchPointIDs[0]];
		Vector2 touchNew = newTouchPoints[touchPointIDs[0]];
		Vector2 delta = touchNew - touchOld;
		delta /= -camera.zoom;
		camera.target += delta;
	}

	if (touchPointIDs.size() == 2) {
		Vector2 touchOld0 = touchPoints[touchPointIDs[0]];
		Vector2 touchOld1 = touchPoints[touchPointIDs[1]];
		Vector2 touchNew0 = newTouchPoints[touchPointIDs[0]];
		Vector2 touchNew1 = newTouchPoints[touchPointIDs[1]];

		Vector2 worldOld0 = GetScreenToWorld2D(touchOld0, camera);
		Vector2 worldOld1 = GetScreenToWorld2D(touchOld1, camera);

		float oldDist = Vector2Distance(touchOld0, touchOld1);
		float newDist = Vector2Distance(touchNew0, touchNew1);

		float zoomFactor = newDist / oldDist;
		float newZoom = camera.zoom * zoomFactor;
		newZoom = Clamp(newZoom, 0.1f, 20.f);
		camera.zoom = newZoom;

		Vector2 worldNew0 = GetScreenToWorld2D(touchNew0, camera);
		Vector2 worldNew1 = GetScreenToWorld2D(touchNew1, camera);

		Vector2 delta0 = worldOld0 - worldNew0;
		Vector2 delta1 = worldOld1 - worldNew1;
		Vector2 avgDelta = (delta0 + delta1) / 2;

		camera.target += avgDelta;
	}

}

void App::handleTouchInputAddBody(std::map<int, Vector2>& touchPoints, std::map<int, Vector2>& newTouchPoints, std::vector<int>& touchPointIDs) {
	if (touchPoints.size() == 0 && newTouchPoints.size() == 1) {
		Vector2 touchPosScreen = newTouchPoints.begin()->second;
		Vector2 touchPosWorld = GetScreenToWorld2D(touchPosScreen, camera);
		// Add new body
		tempBody = new Body(touchPosWorld, Vector2Zero(), 100.f);
		return;
	}

	if (touchPointIDs.size() == 1) {
		Vector2 touchPosScreen = newTouchPoints[touchPointIDs[0]];
		Vector2 touchPosWorld = GetScreenToWorld2D(touchPosScreen, camera);
		tempBody->setVelocity((touchPosWorld - tempBody->getPos()) * -1);
	}

	if (touchPointIDs.size() == 2) {
		Vector2 touchOld0 = touchPoints[touchPointIDs[0]];
		Vector2 touchOld1 = touchPoints[touchPointIDs[1]];
		Vector2 touchNew0 = newTouchPoints[touchPointIDs[0]];
		Vector2 touchNew1 = newTouchPoints[touchPointIDs[1]];

		float oldDist = Vector2Distance(touchOld0, touchOld1);
		float newDist = Vector2Distance(touchNew0, touchNew1);

		float diff = newDist - oldDist;
		float newMass = tempBody->getMass() + diff;
		newMass = Clamp(newMass, 10, 10000);
		tempBody->setMass(newMass);
	}
}
