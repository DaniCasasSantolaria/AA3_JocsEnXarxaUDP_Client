#include "OnlinePlayer.h"
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
	// Si el paquete ya es antiguo
	if (movementId <= lastAppliedMovementId) {
		return;
	}

	MovementReceive newMovement;
	newMovement.movementId = movementId;
	newMovement.position = serverPosition;

	// Ordenamos los movimientos recibidos por el ID para asegurarnos de aplicar el orden correcto y evitar problemas de paquetes desordenados
	std::vector<MovementReceive>::iterator it = pendingReceivedMovements.begin();

	while (it != pendingReceivedMovements.end() && it->movementId < movementId) {
		++it;
	}

	pendingReceivedMovements.insert(it, newMovement);
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

		float distanceToTarget = DistanceSquared(transform->position, target.position);

		float snapDistanceSquared = snapDistance * snapDistance;

		// Si ya estamos muy cerca del target, hacemos TP a la posición exacta
		if (distanceToTarget <= snapDistanceSquared) {
			transform->position = target.position;

			lastAppliedMovementId = target.movementId;

			pendingReceivedMovements.erase(pendingReceivedMovements.begin());
		}
		else {
			// Interpolación hacia la posición recibida del servidor
			transform->position = Lerp(transform->position,	target.position, interpolationFactor);
		}
	}

	Vector2 newPosition = transform->position;

	UpdateAnimationFromMovement(oldPosition, newPosition);
}

void OnlinePlayer::UpdateAnimationFromMovement(const Vector2& oldPosition, const Vector2& newPosition) {
	float deltaX = newPosition.x - oldPosition.x;

	// Como solo recibimos posición, deducimos hacia dónde mira usando el movimiento horizontal
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