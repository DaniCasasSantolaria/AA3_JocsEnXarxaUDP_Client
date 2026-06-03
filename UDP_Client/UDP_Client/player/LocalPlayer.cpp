#include "LocalPlayer.h"
#include "../Managers/InputManager.h"
#include "../Managers/PacketManager.h"
#include "../Spawner.h"
#include "../Elements/Bullet.h"
#include <SFML/Window/Keyboard.hpp>
#include <cmath>

// Movimiento local
void LocalPlayer::ApplyMovementInput(float direction, bool jump, float dt) {
	Vector2 velocity = physics->GetVelocity();

	velocity.x = direction * moveSpeed;

	if (jump && isGrounded) {
		velocity.y = jumpVelocity;
		isGrounded = false;
	}

	physics->SetVelocity(velocity);
}

// Reconciliacion de movimiento con el servidor
void LocalPlayer::ApplyServerValidation(unsigned int movementId, const Vector2& serverPosition) {
	// Ignoramos validaciones antiguas o repetidas
	if (movementId <= lastValidatedMovementId) {
		return;
	}

	lastValidatedMovementId = movementId;

	// Buscamos que posicion habia predicho el cliente para ese mismo movimiento
	Vector2 predictedPosition = serverPosition;
	bool foundPrediction = false;

	for (SentMovement& movement : pendingSentMovements) {
		if (movement.movementId == movementId) {
			predictedPosition = movement.position;
			foundPrediction = true;
			break;
		}
	}

	// Eliminamos los movimientos anteriores al actual
	while (!pendingSentMovements.empty() && pendingSentMovements.front().movementId <= movementId) {
		pendingSentMovements.erase(pendingSentMovements.begin());
	}

	// Si no econtramos prediccion, hacemos tp a la posicion del servidor
	if (!foundPrediction) {
		transform->position = serverPosition;
		pendingSentMovements.clear();
		physics->SetVelocity(Vector2(0.0f, 0.0f));
		return;
	}

	Vector2 correction = serverPosition - predictedPosition;

	// Se comparan las distancias. Parte matematica hecha con IA
	float errorSquared = correction.x * correction.x + correction.y * correction.y;

	const float minCorrection = 1.0f;
	const float snapError = 20.0f;

	// Si el error es demasiado grande, hacemos tp a la posicion del servidor
	if (errorSquared > snapError * snapError) {
		transform->position = serverPosition;
		pendingSentMovements.clear();
		physics->SetVelocity(Vector2(0.0f, 0.0f));
		return;
	}

	// Si el error es pequeno, corregimos suavemente desplazando la posicion actual
	if (errorSquared > minCorrection * minCorrection) {
		transform->position = transform->position + correction;
	}
}

// Movimiento
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

// Disparo
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

	PM->SendShoot(spawnPosition.x, spawnPosition.y, bulletDirection.x, bulletDirection.y);
}

// Burla
void LocalPlayer::Taunt()
{
	if (!Input.GetEvent(sf::Keyboard::Key::O, KeyState::DOWN)) return;

	StartTauntAnimation();
	AUDIO->PlayClip("taunt", 0, 128);
	PM->SendTaunt();
}

void LocalPlayer::Update() {
	if (defeated) {
		physics->SetVelocity(Vector2(0.0f, 0.0f));
		ImageObject::Update();
		return;
	}
	
	Taunt();
	Move();
	ImageObject::Update();
	UpdateTauntAnimation();
	UpdateHitAnimation();
	isGrounded = false;
	Shoot();

}

// Enviamos la posicion al servidor cada cierto tiempo para que pueda validar el movimiento del cliente
void LocalPlayer::TrySendMovement() {
	if (defeated) { return;	}
	
	lastTimeSentMovement += TIME.GetDeltaTime();
 
	if (lastTimeSentMovement >= timeToSendMovement) {
		// Guardamos el movimiento para poder compararlo luego con la validacion del servidor
		SentMovement sentMovement;
		sentMovement.movementId = currentMovementID;
		sentMovement.position = transform->position;

		pendingSentMovements.push_back(sentMovement);

		PM->SendMovement(transform->position.x, transform->position.y, currentMovementID);

		currentMovementID++;
		lastTimeSentMovement -= timeToSendMovement;
	}
}

// Recibir daño
void LocalPlayer::RecieveDamage(short amount) {
	if (defeated || amount <= 0) {
		return;
	}

	currentHealthPoints -= amount;

	StartHitAnimation();

	if (currentHealthPoints > 0) {
		PM->SendLifeHealthUpdate(currentLives, currentHealthPoints);
		SetHealthDebugPrint(true);
		return;
	}

	currentLives--;

	if (currentLives > 0) {
		currentHealthPoints = maxHealthPoints;

		PM->SendLifeHealthUpdate(currentLives, currentHealthPoints);
		SetHealthDebugPrint(true);

		isGrounded = false;

		pendingSentMovements.clear();

		if (currentMovementID > 0) {
			lastValidatedMovementId = currentMovementID - 1;
		}

		lastTimeSentMovement = timeToSendMovement;

		StartHitAnimation();
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
