#pragma once

#include <vector>
#include "Body.hpp"

class World {
private:
	std::vector<Body> bodies;

public:
	World();

	void addBody(Vector2 pos, Vector2 vel, float mass);
	void addRandomBodies(float num, float boundX, float boundY);

	void update(float dt, unsigned int substeps);

	void render();
	void renderLastBodyPath();

	World copy() const;
};
