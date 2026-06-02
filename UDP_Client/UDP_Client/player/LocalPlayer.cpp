#include "LocalPlayer.h"
#include "../Managers/InputManager.h"
#include "../Managers/PacketManager.h"
#include "../Spawner.h"
#include "../Elements/Bullet.h"
#include <SFML/Window/Keyboard.hpp>
#include <cmath>

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

	while (!pendingSentMovements.empty() &&
		pendingSentMovements.front().movementId <= movementId) {
		pendingSentMovements.erase(pendingSentMovements.begin());
	}

	if (!foundPrediction) {
		transform->position = serverPosition;
		pendingSentMovements.clear();
		physics->SetVelocity(Vector2(0.0f, 0.0f));
		return;
	}

	Vector2 correction = serverPosition - predictedPosition;

	float errorSquared = correction.x * correction.x + correction.y * correction.y;

	const float minCorrection = 1.0f;
	const float snapError = 20.0f;

	if (errorSquared > snapError * snapError) {
		transform->position = serverPosition;
		pendingSentMovements.clear();
		physics->SetVelocity(Vector2(0.0f, 0.0f));
		return;
	}

	if (errorSquared > minCorrection * minCorrection) {
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

void LocalPlayer::Shoot() {
	if (!Input.GetEvent(sf::Keyboard::Key::P, KeyState::DOWN)) return;

	float currentTime = TIME.GetElapsedTime();
	if (currentTime - lastTimeShooted < shootCooldown) return;
	lastTimeShooted = currentTime;

	Vector2 bulletDirection = lookingRight ? Vector2(1.0f, 0.0f) : Vector2(-1.0f, 0.0f);

	float spawnOffsetX = (transform->size.x * std::abs(transform->scale.x)) / 2.0f;
	Vector2 spawnPosition = Vector2(
		transform->position.x + bulletDirection.x * spawnOffsetX,
		transform->position.y
	);

	Bullet* bullet = new Bullet(
		"resources/bullet.png",
		bulletDirection,
		PM->GetMyIndex(),
		600.0f
	);
	bullet->GetTransform()->position = spawnPosition;
	SPAWN.SpawnObject(bullet);

	PM->SendShoot(spawnPosition.x, spawnPosition.y, bulletDirection.x, bulletDirection.y);
}

void LocalPlayer::Update() {
	if (defeated) {
		physics->SetVelocity(Vector2(0.0f, 0.0f));
		ImageObject::Update();
		return;
	}

	Move();
	ImageObject::Update();
	isGrounded = false;
	Shoot();
}

void LocalPlayer::TrySendMovement() {
	if (defeated) {
		return;
	}

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

void LocalPlayer::RecieveDamage(short amount) {
	if (defeated || amount <= 0) {
		return;
	}

	currentHealthPoints -= amount;

	ChangeAnimation(PlayerState::HIT);

	if (currentHealthPoints > 0) {
		PM->SendLifeHealthUpdate(currentLives, currentHealthPoints);
		pendingHealthDebugPrint = true;
		return;
	}

	currentLives--;

	if (currentLives > 0) {
		currentHealthPoints = maxHealthPoints;

		PM->SendLifeHealthUpdate(currentLives, currentHealthPoints);
		pendingHealthDebugPrint = true;

		isGrounded = false;

		pendingSentMovements.clear();

		if (currentMovementID > 0) {
			lastValidatedMovementId = currentMovementID - 1;
		}

		lastTimeSentMovement = timeToSendMovement;

		ChangeAnimation(PlayerState::IDLE);
		return;
	}

	currentLives = 0;
	currentHealthPoints = 0;
	defeated = true;

	PM->SendLifeHealthUpdate(currentLives, currentHealthPoints);

	physics->SetVelocity(Vector2(0.0f, 0.0f));
	ChangeAnimation(PlayerState::DEATH);
}

bool LocalPlayer::IsDead() {
	return defeated;
}

bool LocalPlayer::ConsumePendingHealthDebugPrint() {
	if (!pendingHealthDebugPrint) {
		return false;
	}

	pendingHealthDebugPrint = false;
	return true;
}