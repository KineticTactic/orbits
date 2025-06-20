#include "raylib.h"
#include <deque>

#define MAX_PATH_LENGTH 100

class Body {
private:
	Vector2 pos;
	Vector2 vel;
	Vector2 acc;
	Vector2 newAcc;
	float mass;
	float radius;

	std::deque<Vector2> path;

public:
	Body(Vector2 pos, float mass);

	inline void setVelocity(Vector2 v) { vel = v; };

	void applyForce(Vector2 force);
	void update(float dt);
	void beginUpdate(float dt);
	void updateImplicitEuler(float dt);
	void addPath();
	void renderPath();
	void render();

	void springForce();

	static void attract(Body& b1, Body& b2);
	static void collide(Body& b1, Body& b2);
};


