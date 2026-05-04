#include "Spaceship.h"
#include "../Managers/InputManager.h"
#include "../Spawner.h"
#include "../Elements/Bullet.h"

void Spaceship::Move(){
	/*if (Input.GetEvent(SDLK_a, KeyState::DOWN) || Input.GetEvent(SDLK_a, KeyState::HOLD)) {
		physics->AddForce(movementForces[3]);
		physics->SetLinearDrag(20);
	}

	if (Input.GetEvent(SDLK_d, KeyState::DOWN) || Input.GetEvent(SDLK_d, KeyState::HOLD)) {
		physics->AddForce(movementForces[1]);
		physics->SetLinearDrag(20);
	}

	float halfWidth = (transform->size.x / 2.0f) * (transform->scale.x);

	if (transform->position.x <= halfWidth) {
		transform->position.x = halfWidth;
	}
	else if (transform->position.x >= RM->WINDOW_WIDTH - halfWidth) {
		transform->position.x = (RM->WINDOW_WIDTH - halfWidth);
	}*/
}

void Spaceship::Shoot(){
	/*if (Input.GetEvent(SDLK_SPACE, KeyState::DOWN)) {
		Bullet* bullet;
		bullet = new Bullet(SPRITES.GetSelectedPlayerBullet(), Vector2(0.0f, 0.0f), Vector2(16.0f, 16.0f), Vector2(0.0f, -1.0f), "Player", 150.0f);
		if (SPRITES.GetSelectedVFX() == 1) {
			bullet = new Bullet(SPRITES.GetSelectedPlayerBullet(), Vector2(0.0f, 0.0f), Vector2(32.0f, 16.0f), Vector2(0.0f, -1.0f), "Player", 150.0f);
			bullet->GetTransform()->rotation = 270;
		}
		else if (SPRITES.GetSelectedVFX() == 2) {
			bullet->GetTransform()->rotation = 90;
			bullet->GetTransform()->scale = Vector2(1.4f, 1.4f);
		}
		bullet->GetTransform()->position.x = transform->position.x;
		bullet->GetTransform()->position.y = transform->position.y - 24;
		SPAWN.SpawnObject(bullet);

		lastTimeShooted = TIME.GetElapsedTime();
		Player::Shoot();
	}
	else if (Input.GetEvent(SDLK_SPACE, KeyState::HOLD)) {
		if (TIME.GetElapsedTime() - lastTimeShooted > shootCooldown) {
			Bullet* bullet;
			bullet = new Bullet(SPRITES.GetSelectedPlayerBullet(), Vector2(0.0f, 0.0f), Vector2(16.0f, 16.0f), Vector2(0.0f, -1.0f), "Player", 150.0f);
			if (SPRITES.GetSelectedVFX() == 1) {
				bullet = new Bullet(SPRITES.GetSelectedPlayerBullet(), Vector2(0.0f, 0.0f), Vector2(32.0f, 16.0f), Vector2(0.0f, -1.0f), "Player", 150.0f);
				bullet->GetTransform()->rotation = 270;
			}
			else if (SPRITES.GetSelectedVFX() == 2)
				bullet->GetTransform()->rotation = 90;

			bullet->GetTransform()->position.x = transform->position.x;
			bullet->GetTransform()->position.y = transform->position.y - 24;
			SPAWN.SpawnObject(bullet);

			lastTimeShooted = TIME.GetElapsedTime();
			Player::Shoot();
		}
	}
	*/
}