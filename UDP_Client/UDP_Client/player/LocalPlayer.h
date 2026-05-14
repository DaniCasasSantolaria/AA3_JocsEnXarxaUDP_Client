#pragma once
#include "Player.h"

class LocalPlayer : public Player {
public:
	LocalPlayer() = default;
	LocalPlayer(std::string texturepath, Vector2 sourceOffset, Vector2 sourceSize, unsigned short numRows, unsigned short numColumns, 
		float frameTime, bool hasToLoop, float shootCooldown, short life)
		: Player(texturepath, sourceOffset, sourceSize, numRows, numColumns, frameTime, hasToLoop, shootCooldown, life) {
		this->GetRigidbody()->SetGravity(gravity);
	}

	void Move() override;
	void Update() override;
};