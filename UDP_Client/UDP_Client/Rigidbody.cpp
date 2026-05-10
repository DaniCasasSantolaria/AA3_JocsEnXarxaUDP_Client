#include "Rigidbody.h"
#include "Managers/TimeManager.h"
#include <cmath>

bool Rigidbody::CheckCollision(const Rigidbody* rb) {
	for (AABB* myCol : colliders) {
		for (AABB* otherCol : rb->colliders) {
			if (myCol->CheckOverlappingAABB(otherCol))
				return true;
		}
	}
	return false;
}

bool Rigidbody::CheckOverlappingPoint(Vector2 point) {
	for (AABB* myCol : colliders) {
		if (myCol->CheckOverlappingPoint(point))
			return true;
	}
	return false;
}

void Rigidbody::Update(float dt) {
	//1 -> Update velocity
	acceleration += Vector2(0, gravity);
	velocity = velocity + (acceleration * dt);
	angularVelocity = angularVelocity + angularAcceleration * dt;

	//2 -> Update the DRAG
	velocity = velocity * (1.0f / (1.0f + dt * linearDrag));
	angularVelocity = angularVelocity * (1.0f / (1.0f + dt * angularDrag));

	//3 -> Update position
	transform->position = transform->position + velocity * dt;
	transform->rotation = transform->rotation + angularVelocity * dt;

	//4 -> Reset acceleration
	acceleration = Vector2();
	angularAcceleration = 0.0f;

	//5 -> Update the colliders positioning
	Vector2 absScale = Vector2(
		std::abs(transform->scale.x),
		std::abs(transform->scale.y)
	);

	Vector2 colliderSize = transform->size * absScale;
	Vector2 offset = Vector2(
		-colliderSize.x / 2.0f,
		-colliderSize.y / 2.0f
	);

	for (AABB* col : colliders) {
		col->SetTopLeft(transform->position + offset);
		col->SetSize(colliderSize);
	}
}