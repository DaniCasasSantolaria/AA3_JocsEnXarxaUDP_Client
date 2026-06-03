#pragma once
#include <vector>
#include "AABB.h"
#include "Transform.h"

class Rigidbody{
private:
	std::vector<AABB*> colliders;
	Transform* transform;
	Vector2 velocity;
	float angularVelocity;
	Vector2 acceleration;
	float angularAcceleration;
	float linearDrag;
	float angularDrag;
	float gravity = 0.0f;
public:
	Rigidbody(Transform* transform)
		: transform(transform),
		velocity(Vector2()), angularVelocity(0.0f),
		acceleration(Vector2()), angularAcceleration(0.0f),
		linearDrag(0.0f), angularDrag(0.0f) {}
	inline void AddCollider(AABB* collider) { colliders.push_back(collider); }
	inline void ClearColliders() { colliders.clear(); }
	bool CheckCollision(const Rigidbody* rb);
	bool CheckOverlappingPoint(Vector2 point);
	void Update(float dt);
	inline void AddForce(Vector2 force) { acceleration += force; }
	inline void AddTorque(float amount) { angularAcceleration += amount; }
	inline Vector2 GetVelocity() const { return velocity; };
	inline void SetVelocity(Vector2 velocity) { this->velocity = velocity; }
	inline void SetLinearDrag(float linearDrag) { this->linearDrag = linearDrag; }
	inline void SetAngularDrag(float angularDrag) { this->angularDrag = angularDrag; }
	inline void SetGravity(float gravity) { this->gravity = gravity; }

	~Rigidbody() {
		for (AABB* col : colliders)
			delete col;
	}
};