#include "LocalPlayer.h"
#include "../Managers/InputManager.h"
#include <SFML/Window/Keyboard.hpp>
#include <cmath>
#include "../Managers/PacketManager.h"

void LocalPlayer::ApplyMovementInput(float direction, bool jump, float dt) {
	Vector2 velocity = physics->GetVelocity();

	velocity.x = direction * moveSpeed;

	if (jump && isGrounded) {
		velocity.y = jumpVelocity;
		isGrounded = false;
	}

	physics->SetVelocity(velocity);
}

void LocalPlayer::ApplyServerValidation(unsigned int movementId, const Vector2& serverPosition) {
	// Ignorar validaciones antiguas
	if (movementId <= lastValidatedMovementId) {
		return;
	}

	lastValidatedMovementId = movementId;

	Vector2 predictedPosition = serverPosition;
	bool foundPrediction = false;

	for (SentMovement& movement : pendingSentMovements) {
		if (movement.movementId == movementId) {
			predictedPosition = movement.position;
			foundPrediction = true;
			break;
		}
	}

	// Borrar movimientos confirmados
	while (!pendingSentMovements.empty() &&
		pendingSentMovements.front().movementId <= movementId) {
		pendingSentMovements.erase(pendingSentMovements.begin());
	}

	Vector2 correction = serverPosition - predictedPosition;

	float errorSquared = correction.x * correction.x + correction.y * correction.y;

	float minCorrection = 1.0f;
	float maxSnapError = 250.0f;

	if (errorSquared > maxSnapError * maxSnapError) {
		transform->position = serverPosition;
	}
	else if (foundPrediction && errorSquared > minCorrection * minCorrection) {
		transform->position = transform->position + correction;
	}
}

void LocalPlayer::Move() {
	Vector2 velocity = physics->GetVelocity();

	float direction = 0.0f;

	if (Input.GetEvent(sf::Keyboard::Key::A, KeyState::DOWN) ||
		Input.GetEvent(sf::Keyboard::Key::A, KeyState::HOLD)) {
		lookingRight = false;
		direction -= 1.0f;
	}

	if (Input.GetEvent(sf::Keyboard::Key::D, KeyState::DOWN) ||
		Input.GetEvent(sf::Keyboard::Key::D, KeyState::HOLD)) {
		lookingRight = true;
		direction += 1.0f;
	}

	velocity.x = direction * moveSpeed;

	bool jumpPressed = false;

	if (Input.GetEvent(sf::Keyboard::Key::Space, KeyState::DOWN) && isGrounded) {
		velocity.y = jumpVelocity;
		isGrounded = false;
		jumpPressed = true;
	}

	physics->SetVelocity(velocity);

	if (lookingRight) {
		transform->scale.x = std::abs(transform->scale.x);
	}
	else {
		transform->scale.x = -std::abs(transform->scale.x);
	}

	if (direction != 0.0f) {
		ChangeAnimation(PlayerState::MOVE);
	}
	else {
		ChangeAnimation(PlayerState::IDLE);
	}
}

void LocalPlayer::Update() {
	Move();
	ImageObject::Update();
	isGrounded = false;
	Shoot();
}

void LocalPlayer::TrySendMovement() {
	lastTimeSentMovement += TIME.GetDeltaTime();

	if (lastTimeSentMovement >= timeToSendMovement) {
		SentMovement sentMovement;
		sentMovement.movementId = currentMovementID;
		sentMovement.position = transform->position;

		pendingSentMovements.push_back(sentMovement);

		PM->SendMovement(transform->position.x, transform->position.y, currentMovementID);

		currentMovementID++;
		lastTimeSentMovement -= timeToSendMovement;
	}
}