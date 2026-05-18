#pragma once
#include "Player.h"

class LocalPlayer : public Player {
private:
	//UDP movimiento
	unsigned int currentMovementID = 0;
	float timeToSendMovement = 0.2f;
	float lastTimeSentMovement = 0.0f;

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