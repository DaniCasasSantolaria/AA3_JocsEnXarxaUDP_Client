#include "OnlinePlayer.h"
#include "../Managers/TimeManager.h"
#include <cmath>

// Funciones matemáticas de Lerp y DistanceSquared hechas con IA
Vector2 OnlinePlayer::Lerp(const Vector2& a, const Vector2& b, float t) {
	return Vector2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
}

float OnlinePlayer::DistanceSquared(const Vector2& a, const Vector2& b) {
	float dx = a.x - b.x;
	float dy = a.y - b.y;

	return dx * dx + dy * dy;
}

void OnlinePlayer::AddServerMovement(unsigned int movementId, const Vector2& serverPosition) {
	if (movementId <= lastAppliedMovementId) {
		return;
	}

	pendingReceivedMovements.clear();

	MovementReceive newMovement;
	newMovement.movementId = movementId;
	newMovement.position = serverPosition;

	pendingReceivedMovements.push_back(newMovement);
}

void OnlinePlayer::Move() {
	physics->SetVelocity(Vector2(0.0f, 0.0f));

	Vector2 oldPosition = transform->position;

	// Eliminamos movimientos antiguos que hayan quedado
	while (!pendingReceivedMovements.empty() &&	pendingReceivedMovements.front().movementId <= lastAppliedMovementId) {
		pendingReceivedMovements.erase(pendingReceivedMovements.begin());
	}

	if (!pendingReceivedMovements.empty()) {
		MovementReceive& target = pendingReceivedMovements.front();

		Vector2 direction = target.position - transform->position;

		float distance = std::sqrt(
			direction.x * direction.x +
			direction.y * direction.y
		);

		float maxStep = moveSpeed * TIME.GetDeltaTime();

		if (distance <= maxStep || distance <= 0.001f) {
			transform->position = target.position;

			lastAppliedMovementId = target.movementId;

			pendingReceivedMovements.erase(pendingReceivedMovements.begin());
		}
		else {
			Vector2 normalizedDirection = Vector2(
				direction.x / distance,
				direction.y / distance
			);

			transform->position = transform->position + normalizedDirection * maxStep;
		}
	}

	Vector2 newPosition = transform->position;

	UpdateAnimationFromMovement(oldPosition, newPosition);
}

void OnlinePlayer::UpdateAnimationFromMovement(const Vector2& oldPosition, const Vector2& newPosition) {
	float deltaX = newPosition.x - oldPosition.x;

	// Decidimos hacia dónde mira usando el movimiento horizontal
	if (std::abs(deltaX) > animationMoveThreshold) {
		lookingRight = deltaX > 0.0f;
	}

	if (lookingRight) {
		transform->scale.x = std::abs(transform->scale.x);
	}
	else {
		transform->scale.x = -std::abs(transform->scale.x);
	}

	if (std::abs(deltaX) > animationMoveThreshold) {
		ChangeAnimation(PlayerState::MOVE);
	}
	else {
		ChangeAnimation(PlayerState::IDLE);
	}
}

void OnlinePlayer::Update() {
	Move();
	ImageObject::Update();
}