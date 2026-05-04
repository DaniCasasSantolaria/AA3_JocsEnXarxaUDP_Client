#pragma once
#include "Player.h"

class Spaceship : public Player {
public:
	Spaceship(std::string texturepath, Vector2 sourceOffset, Vector2 sourceSize, float shootCooldown, int life)
		: Player(texturepath, sourceOffset, sourceSize, shootCooldown, life) {}

	void Move() override;
	void Shoot() override;
};