#pragma once

#include <map>
#include <raylib.h>
#include "World.hpp"
#include "GUI.hpp"

class App {
	private:
	Camera2D camera;
	World world;
	World* tempWorld;
	Body* tempBody;
	GUI gui;

	unsigned int frame;
	float zoomSpeed;

	std::map<int, Vector2> touchPoints;

	public:
	App();

	void handleTouchInputCamera(std::map<int, Vector2>& touchPoints, std::map<int, Vector2>& newTouchPoints, std::vector<int>& touchPointIDs);
	void handleTouchInputAddBody(std::map<int, Vector2>& touchPoints, std::map<int, Vector2>& newTouchPoints, std::vector<int>& touchPointIDs);
	void handleInput();
	void update();
	void render();
};
