#include "Body.hpp"
#include "raylib.h"
#include "raymath.h"
#include <cstddef>
//#include "raylibVectorOverloads.hpp"

Body::Body(Vector2 pos, Vector2 vel, float mass)
	: pos(pos),
	  vel(vel),
	  acc({0.f, 0.f}),
newAcc({0.f, 0.f}),
mass(mass) {
	radius = sqrt(mass);
};

void Body::applyForce(Vector2 force) {
	// F = ma
	Vector2 accDueToForce = force / mass;
	// We have to store new acc separately because verlet needs both
	// previous value and new value of acceleration per time step.
	newAcc = newAcc + accDueToForce;
}

// Semi-implicit euler (we aren't using this function)
void Body::update(float dt) {
	acc = newAcc;
	vel = vel + acc * dt;
	pos = pos + vel * dt;
	newAcc = Vector2Zero();
}

// Verlet step 1: Calculate new position from previous step data.
void Body::beginUpdateVerlet(float dt) {
	// 2nd law of motion: x = x0 + ut + 1/2 a t2
	pos = pos + (vel * dt) + (acc * dt * dt * 0.5);
}

// Verlet step 2: Calculate the new value of acceleration
// All the gravity force calculation happens here

// Verlet step 3: Calculate new value of velocity.
void Body::endUpdateVerlet(float dt) {
	// Uses the AVERAGE of the previous and current accelerations.
	// v = u + (a avg) t;
	Vector2 newVel = vel + (acc + newAcc) * 0.5 * dt;

	// Update variables for next timestep.
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
	Color col = ColorFromHSV(mass / 10000 * 360, 0.8, 0.5);
	for (size_t i = 1; i < path.size(); ++i) {
		float alpha = static_cast<float>(i) / path.size() / 2;
		DrawSplineSegmentLinear(path[i - 1], path[i], alpha * radius, Fade(col, alpha));
	}
}

void Body::renderPathAsFuture() {
	for(size_t i = 0; i < path.size(); i++) {
		float alpha = static_cast<float>(path.size() - i) / path.size() / 2;
		DrawCircleV(path[i], 5, Fade(YELLOW,alpha));
	}
}

void Body::render() {
	Color col = ColorFromHSV(mass / 10000 * 360, 0.8, 1.0);
	DrawCircleV(pos, radius, col);
}

void Body::attract(Body &b1, Body &b2) {
	const float G = 3000;
	Vector2 r = b2.pos - b1.pos; // Vector directed from 1 to 2
	float distSq = Vector2LengthSqr(r);

	// BEHOLD, the greatest formula of all time!
	float forceMag = G * b1.mass * b2.mass / distSq;

	Vector2 force1 = Vector2Normalize(r) * forceMag;
	Vector2 force2 = force1 * -1;

	b1.applyForce(force1);
	b2.applyForce(force2);
}

void Body::collide(Body &b1, Body &b2) {
	Vector2 r = b2.pos - b1.pos;
	float dist = Vector2Length(r);

	// If not overlapping, return.
	if(dist >= b1.radius + b2.radius) return;

	// They are overlapping.
	// Determine the offsets by which the balls need to be moved
	float overlap = b1.radius + b2.radius - dist;
	Vector2 rNorm = r / dist; // Normalized r vector
	// Move each ball by half of overlap
	Vector2 offset1 = rNorm * -overlap / 2.f;
	Vector2 offset2 = rNorm * overlap / 2.f;
	b1.pos += offset1;
	b2.pos += offset2;

	// Calculate velocity of Center of mass
	Vector2 vCOM = (b1.vel * b1.mass + b2.vel * b2.mass) / (b1.mass + b2.mass);

	// Calculate tangential velocity (component of v along r)
	float vTangent1 = Vector2DotProduct(b1.vel, r) / dist;
	float vTangent2 = Vector2DotProduct(b2.vel, r) / dist;
	float vCOMTangent = Vector2DotProduct(vCOM, r) / dist;

	// Now we can subtract the tangential component of the velocity.
	// Add a multiplier for bounciness
	/// TODO: Use actual restitution formula
	float bounciness = 1.7f;
	b1.vel = b1.vel - rNorm * (vTangent1 - vCOMTangent) * bounciness;
	b2.vel = b2.vel - rNorm * (vTangent2 - vCOMTangent) * bounciness;
}
