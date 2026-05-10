#include "Player.h"
#include "../Managers/InputManager.h"
#include <SFML/Window/Keyboard.hpp>
#include <cmath>

void Player::OnCollisionEnter(Object* other) {
	if (Bullet* bullet = dynamic_cast<Bullet*>(other)) {
		if (bullet->GetShooter() == "Enemy") {
			RecieveDamage(1);
			AUDIO->PlayClip("hit_enemy", 0, 80);
			other->Destroy();
		}

		return;
	}

	if (dynamic_cast<ImageObject*>(other)) {
		ResolveSolidCollision(other);
	}
}

void Player::Move() {
	Vector2 velocity = physics->GetVelocity();

	float direction = 0.0f;

	if (Input.GetEvent(sf::Keyboard::Key::A, KeyState::DOWN) ||
		Input.GetEvent(sf::Keyboard::Key::A, KeyState::HOLD)) {
		direction -= 1.0f;
	}

	if (Input.GetEvent(sf::Keyboard::Key::D, KeyState::DOWN) ||
		Input.GetEvent(sf::Keyboard::Key::D, KeyState::HOLD)) {
		direction += 1.0f;
	}

	velocity.x = direction * moveSpeed;

	if (Input.GetEvent(sf::Keyboard::Key::Space, KeyState::DOWN) && isGrounded) {
		velocity.y = jumpVelocity;
		isGrounded = false;
	}

	physics->SetVelocity(velocity);
}

void Player::ResolveSolidCollision(Object* other) {
	Transform* otherTransform = other->GetTransform();

	Vector2 playerHalfSize = (transform->size * transform->scale) / 2.0f;
	Vector2 otherHalfSize = (otherTransform->size * otherTransform->scale) / 2.0f;

	float deltaX = transform->position.x - otherTransform->position.x;
	float deltaY = transform->position.y - otherTransform->position.y;

	float overlapX = playerHalfSize.x + otherHalfSize.x - std::abs(deltaX);
	float overlapY = playerHalfSize.y + otherHalfSize.y - std::abs(deltaY);

	if (overlapX <= 0.0f || overlapY <= 0.0f) {
		return;
	}

	Vector2 velocity = physics->GetVelocity();

	if (overlapX < overlapY) {
		if (deltaX > 0.0f) {
			transform->position.x += overlapX;
		}
		else {
			transform->position.x -= overlapX;
		}

		velocity.x = 0.0f;
	}
	else {
		if (deltaY > 0.0f) {
			transform->position.y += overlapY;
		}
		else {
			transform->position.y -= overlapY;
			isGrounded = true;
		}

		velocity.y = 0.0f;
	}

	physics->SetVelocity(velocity);
}