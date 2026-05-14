#include "LocalPlayer.h"
#include "../Managers/InputManager.h"
#include <SFML/Window/Keyboard.hpp>
#include <cmath>

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

	if (Input.GetEvent(sf::Keyboard::Key::Space, KeyState::DOWN) && isGrounded) {
		velocity.y = jumpVelocity;
		isGrounded = false;
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
	isGrounded = false;

	ImageObject::Update();

	Shoot();
}
