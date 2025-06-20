#include "Body.hpp"
#include "raylib.h"
#include "raymath.h"
#include <iostream>

Body::Body(Vector2 pos, float mass)
	: pos(pos),
	  vel({0.f, 0.f}),
acc({0.f, 0.f}),
newAcc({0.f, 0.f}),
mass(mass) {
	//vel.x =GetRandomValue(-100, 100);
	//vel.y = GetRandomValue(-100, 100);
};

void Body::applyForce(Vector2 force) {
	Vector2 accDueToForce = Vector2Scale(force, 1.f / mass);
	newAcc = Vector2Add(newAcc, accDueToForce);
}

void Body::update(float dt) {
	acc = newAcc;
	vel = Vector2Add(vel, Vector2Scale(acc, dt));
	pos = Vector2Add(pos, Vector2Scale(vel, dt));
	newAcc = Vector2Zero();
}

void Body::beginUpdate(float dt) {
	pos = Vector2Add(Vector2Add(pos, Vector2Scale(vel, dt)), Vector2Scale(acc, 0.5 * dt * dt));
}

void Body::updateImplicitEuler(float dt) {
	Vector2 newVel = Vector2Add(vel, Vector2Scale(Vector2Add(acc, newAcc), 0.5 * dt));
	vel = newVel;
	acc = newAcc;
	newAcc = Vector2Zero();
}


void Body::render() {
	DrawCircleV(pos, sqrtf(mass), GREEN);
}

void Body::springForce() {
	float k = 1;
	float x = 200 - pos.x;
	Vector2 force = Vector2Scale(Vector2{1.0, 0.0}, k * x);
	applyForce(force);
}

void Body::attract(Body &b1, Body &b2) {
	const float G = 1000;
	Vector2 vecFrom1To2 = Vector2Subtract(b2.pos, b1.pos);
	float distSq = Vector2LengthSqr(vecFrom1To2);
	float forceMag = G * b1.mass * b2.mass / distSq;

	Vector2 force1 = Vector2Scale(Vector2Normalize(vecFrom1To2), forceMag);
	Vector2 force2 = Vector2Scale(force1, -1.f);

	b1.applyForce(force1);
	b2.applyForce(force2);
}
