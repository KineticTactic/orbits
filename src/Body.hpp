#include "raylib.h"
#include <cmath>
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

	inline Vector2 getPos() {return pos;}
	inline Vector2 getVel() {return vel;}
	inline float getMass() { return mass;}
	inline void setVelocity(Vector2 v) { vel = v; };
	inline void setMass(float m) { mass = m; radius = sqrt(m);}

	void applyForce(Vector2 force);
	void update(float dt);
	void beginUpdateVerlet(float dt);
	void endUpdateVerlet(float dt);
	void addPath();
	void renderPath();
	void render();

	void springForce();

	static void attract(Body& b1, Body& b2);
	static void collide(Body& b1, Body& b2);
};


