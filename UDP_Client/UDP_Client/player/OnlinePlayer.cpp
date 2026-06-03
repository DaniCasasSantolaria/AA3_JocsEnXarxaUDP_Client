#include "OnlinePlayer.h"
#include "../Managers/TimeManager.h"
#include <cmath>
#include <algorithm>

void OnlinePlayer::AddServerMovement(unsigned int movementId, const Vector2& serverPosition) {
	if (movementId <= lastAppliedMovementId) {
		return;
	}

	MovementReceive newMovement;
	newMovement.movementId = movementId;
	newMovement.position = serverPosition;

	pendingReceivedMovements.push_back(newMovement);

	//HECHO CON IA PARA ORDENAR LOS MOVIMIENTOS POR ID, ASI SE APLICAN EN ORDEN CORRECTO
	std::sort(pendingReceivedMovements.begin(), pendingReceivedMovements.end(),
		[](const MovementReceive& a, const MovementReceive& b) {
			return a.movementId < b.movementId;
		}
	);

	const unsigned short maxBufferedMovements = 8;

	while (pendingReceivedMovements.size() > maxBufferedMovements) {
		pendingReceivedMovements.erase(pendingReceivedMovements.begin());
	}
}

void OnlinePlayer::Move() {
	physics->SetVelocity(Vector2(0.0f, 0.0f));

	Vector2 oldPosition = transform->position;

	while (!pendingReceivedMovements.empty() &&	pendingReceivedMovements.front().movementId <= lastAppliedMovementId) {
		pendingReceivedMovements.erase(pendingReceivedMovements.begin());
	}

	const unsigned short maxDelayMovements = 4;

	while (pendingReceivedMovements.size() > maxDelayMovements) {
		lastAppliedMovementId = pendingReceivedMovements.front().movementId;
		pendingReceivedMovements.erase(pendingReceivedMovements.begin());
	}

	if (!pendingReceivedMovements.empty()) {
		MovementReceive* target = &pendingReceivedMovements.front();

		float dx = target->position.x - transform->position.x;
		float maxStepX = moveSpeed * TIME.GetDeltaTime();

		if (std::abs(dx) <= maxStepX) {
			transform->position.x = target->position.x;
		}
		else {
			transform->position.x += (dx > 0.0f ? 1.0f : -1.0f) * maxStepX;
		}

		float dy = target->position.y - transform->position.y;
		float maxStepY = moveSpeedVertical * TIME.GetDeltaTime();

		if (std::abs(dy) <= maxStepY) {
			transform->position.y = target->position.y;
		}
		else {
			transform->position.y += (dy > 0.0f ? 1.0f : -1.0f) * maxStepY;
		}

		float remainingDx = target->position.x - transform->position.x;
		float remainingDy = target->position.y - transform->position.y;

		if ((remainingDx * remainingDx + remainingDy * remainingDy) <= 1.0f) {
			transform->position = target->position;
			lastAppliedMovementId = target->movementId;
			pendingReceivedMovements.erase(pendingReceivedMovements.begin());
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
	UpdateTauntAnimation();
	UpdateHitAnimation();
}