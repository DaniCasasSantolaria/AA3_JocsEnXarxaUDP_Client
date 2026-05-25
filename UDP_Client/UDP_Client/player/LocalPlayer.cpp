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

	physics->Update(dt);
}

void LocalPlayer::ApplyServerValidation(unsigned int movementId, const Vector2& serverPosition) {
	// Ignorar validaciones antiguas
	if (movementId <= lastValidatedMovementId) {
		return;
	}

	lastValidatedMovementId = movementId;

	// Borrar movimientos confirmados
	while (!pendingSentMovements.empty() &&
		pendingSentMovements.front().movementId <= movementId) {
		pendingSentMovements.erase(pendingSentMovements.begin());
	}

	// TP del player a la posición válida del servidor
	transform->position = serverPosition;

	// Reaplicar movimientos que el cliente ya ha hecho, pero el servidor aún no ha confirmado
	for (SentMovement& movement : pendingSentMovements) {
		ApplyMovementInput(movement.direction, movement.jump, movement.deltaTime);
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

	currentInputDirection = direction;
	currentJumpInput = jumpPressed;

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
	isGrounded = false;

	lastTimeSentMovement += TIME.GetDeltaTime();

	if (lastTimeSentMovement >= timeToSendMovement) {
		SentMovement sentMovement;
		sentMovement.movementId = currentMovementID;
		sentMovement.direction = currentInputDirection;
		sentMovement.jump = currentJumpInput;
		sentMovement.deltaTime = lastTimeSentMovement;
		sentMovement.position = transform->position;
		sentMovement.velocity = physics->GetVelocity();

		pendingSentMovements.push_back(sentMovement);

		PM->SendMovement(transform->position.x, transform->position.y, currentMovementID);

		currentMovementID++;
		lastTimeSentMovement = 0.0f;
	}

	ImageObject::Update();

	Shoot();
}
