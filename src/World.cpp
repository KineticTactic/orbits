#include "World.hpp"

World::World() {

}

void World::addBody(Vector2 pos, Vector2 vel, float mass) {
	bodies.emplace_back(pos, vel, mass);
}

void World::addRandomBodies(float num, float boundX, float boundY) {
	for (int i = 0; i < num; ++i) {
		float x = static_cast<float>(GetRandomValue(0, boundX));
		float y = static_cast<float>(GetRandomValue(0, boundY));
		float m = static_cast<float>(GetRandomValue(10, 1000));

		float v = 200.f;
		float velX = static_cast<float>(GetRandomValue(-v, v));
		float velY = static_cast<float>(GetRandomValue(-v, v));

		bodies.emplace_back(Vector2{ x, y }, Vector2{velX, velY}, m);
	}
}

void World::update(float dt, unsigned int substeps) {
	for(int n = 0; n < substeps; n++) {
		for (Body& body : bodies)
			body.beginUpdateVerlet(dt / substeps);

		for (size_t i = 0; i < bodies.size(); ++i) {
			for (size_t j = i + 1; j < bodies.size(); ++j)
				Body::attract(bodies[i], bodies[j]);
		}

		for (Body& body : bodies)
			body.endUpdateVerlet(dt / substeps);

		for (size_t i = 0; i < bodies.size(); ++i) {
			for (size_t j = i + 1; j < bodies.size(); ++j)
				Body::collide(bodies[i], bodies[j]);
		}
	}
}

void World::addPath(bool onlyLast) {
	if(onlyLast) {
		bodies[bodies.size() - 1].addPath();
		return;
	}
	for (Body& body : bodies) body.addPath();
}

void World::render() {
		for(Body& body: bodies) 
			body.renderPath();

		for(Body& body : bodies) 
			body.render();
}

void World::renderLastBodyPath() {
	bodies[bodies.size() - 1].renderPathAsFuture();
}

World* World::copy() const {
	World* w = new World();
	w->bodies = this->bodies;
	return w;
}
