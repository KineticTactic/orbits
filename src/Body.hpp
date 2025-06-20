#include "raylib.h"

class Body {
private:
	Vector2 pos;
	Vector2 vel;
	Vector2 acc;
	Vector2 newAcc;
	float mass;

public:
	Body(Vector2 pos, float mass);

	inline void setVelocity(Vector2 v) { vel = v; };

	void applyForce(Vector2 force);
	void update(float dt);
	void beginUpdate(float dt);
	void updateImplicitEuler(float dt);
	void render();

	void springForce();

	static void attract(Body& b1, Body& b2);
};
