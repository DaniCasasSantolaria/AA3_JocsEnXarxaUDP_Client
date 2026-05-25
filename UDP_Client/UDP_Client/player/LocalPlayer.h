#pragma once
#include "Player.h"

class LocalPlayer : public Player {
private:
	//UDP movimiento
	unsigned int currentMovementID = 0;
	float timeToSendMovement = 0.025f;
	float lastTimeSentMovement = 0.0f;

	struct SentMovement {
		unsigned int movementId = 0;
		float direction = 0.0f;
		bool jump = false;
		float deltaTime = 0.0f;

		Vector2 position = Vector2(0.0f, 0.0f); // opcional/debug
		Vector2 velocity = Vector2(0.0f, 0.0f); // opcional/debug
	};

	unsigned int lastValidatedMovementId = 0;

	float currentInputDirection = 0.0f;
	bool currentJumpInput = false;

	std::vector<SentMovement> pendingSentMovements;

public:
	LocalPlayer() = default;
	LocalPlayer(std::string texturepath, Vector2 sourceOffset, Vector2 sourceSize, unsigned short numRows, unsigned short numColumns, 
		float frameTime, bool hasToLoop, float shootCooldown, short life)
		: Player(texturepath, sourceOffset, sourceSize, numRows, numColumns, frameTime, hasToLoop, shootCooldown, life, true) {
		this->GetRigidbody()->SetGravity(gravity);
	}

	void ApplyMovementInput(float direction, bool jump, float dt);
	void ApplyServerValidation(unsigned int movementId, const Vector2& serverPosition);

	void Move() override;
	void Update() override;
};