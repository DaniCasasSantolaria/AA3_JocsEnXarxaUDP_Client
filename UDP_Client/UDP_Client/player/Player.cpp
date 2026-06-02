#include "Player.h"
#include "../Managers/InputManager.h"
#include "../Managers/PacketManager.h"
#include <SFML/Window/Keyboard.hpp>
#include <cmath>

void Player::OnCollisionEnter(Object* other) {
	if (dynamic_cast<Bullet*>(other)) {
		return;
	}

	if (dynamic_cast<Player*>(other)) {
		return;
	}

	if (dynamic_cast<ImageObject*>(other)) {
		ResolveSolidCollision(other);
	}
}

void Player::ChangeAnimation(PlayerState newState) {
	if (currentState == newState) {
		return;
	}

	currentState = newState;

	AnimatedImageRenderer* animatedRenderer =
		dynamic_cast<AnimatedImageRenderer*>(renderer);

	if (animatedRenderer == nullptr) {
		return;
	}

	switch (currentState) {
	case PlayerState::IDLE:
		animatedRenderer->SetAnimation(0, 4);
		break;

	case PlayerState::MOVE:
		animatedRenderer->SetAnimation(2, 8);
		break;

	case PlayerState::ROLL:
		animatedRenderer->SetAnimation(3, 8);
		break;

	case PlayerState::HIT:
		animatedRenderer->SetAnimation(4, 4);
		break;
	case PlayerState::DEATH:
		animatedRenderer->SetAnimation(5, 4);
		break;
	}
}

void Player::ResolveSolidCollision(Object* other) {
	// HECHO CON IA PARA QUE NO ATRAVIESE EL SUELO NI LOS MUROS
	Transform* otherTransform = other->GetTransform();

	Vector2 playerScale = Vector2(
		std::abs(transform->scale.x),
		std::abs(transform->scale.y)
	);

	Vector2 otherScale = Vector2(
		std::abs(otherTransform->scale.x),
		std::abs(otherTransform->scale.y)
	);

	Vector2 playerHalfSize = (transform->size * playerScale) / 2.0f;
	Vector2 otherHalfSize = (otherTransform->size * otherScale) / 2.0f;

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
