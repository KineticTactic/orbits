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
	radius = sqrt(mass);
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

void Body::beginUpdateVerlet(float dt) {
	pos = Vector2Add(Vector2Add(pos, Vector2Scale(vel, dt)), Vector2Scale(acc, 0.5 * dt * dt));
}

void Body::endUpdateVerlet(float dt) {
	Vector2 newVel = Vector2Add(vel, Vector2Scale(Vector2Add(acc, newAcc), 0.5 * dt));
	vel = newVel;
	acc = newAcc;
	newAcc = Vector2Zero();

}

void Body::addPath() {
	path.push_back(pos);
	if (path.size() > MAX_PATH_LENGTH)
		path.pop_front();

}

void Body::renderPath() {
	for (size_t i = 1; i < path.size(); ++i) {
		DrawSplineSegmentLinear(path[i - 1], path[i], 10, Fade(BLUE, static_cast<float>(i) / path.size() / 2));
	}
}

void Body::render() {
	DrawCircleV(pos, radius, GREEN);
}

void Body::springForce() {
	float k = 1;
	float x = 200 - pos.x;
	Vector2 force = Vector2Scale(Vector2{1.0, 0.0}, k * x);
	applyForce(force);
}

void Body::attract(Body &b1, Body &b2) {
	const float G = 3000;
	Vector2 vecFrom1To2 = Vector2Subtract(b2.pos, b1.pos);
	float distSq = Vector2LengthSqr(vecFrom1To2);
	float forceMag = G * b1.mass * b2.mass / distSq;

	Vector2 force1 = Vector2Scale(Vector2Normalize(vecFrom1To2), forceMag);
	Vector2 force2 = Vector2Scale(force1, -1.f);

	b1.applyForce(force1);
	b2.applyForce(force2);
}

void Body::collide(Body &b1, Body &b2) {
	Vector2 r = Vector2Subtract(b2.pos, b1.pos);
	float dist = Vector2Length(r);

	if(dist >= b1.radius + b2.radius) return;

	// They are overlapping.
	// Determine the offsets by which the balls need to be moved
	float overlap = b1.radius + b2.radius - dist;
	Vector2 offset1 = Vector2Scale(r, -overlap / 2.f / dist);
	Vector2 offset2 = Vector2Scale(r, overlap / 2.f / dist);
	b1.pos = Vector2Add(b1.pos, offset1);
	b2.pos = Vector2Add(b2.pos, offset2);

	Vector2 vCOM = Vector2Scale(Vector2Add(Vector2Scale(b1.vel, b1.mass), Vector2Scale(b2.vel, b2.mass)), 1 / (b1.mass + b2.mass));

	// Calculate tangential velocity
	float vTangent1 = Vector2DotProduct(b1.vel, r) / dist;
	float vTangent2 = Vector2DotProduct(b2.vel, r) / dist;
	float vCOMTangent = Vector2DotProduct(vCOM, r) / dist;

	// Now we can subtract the tangential component of the velocity.
	b1.vel = Vector2Subtract(b1.vel, Vector2Scale(r, (vTangent1 - vCOMTangent) / dist * 1.7f));
	b2.vel = Vector2Subtract(b2.vel, Vector2Scale(r, (vTangent2 - vCOMTangent) / dist * 1.7f));

}
