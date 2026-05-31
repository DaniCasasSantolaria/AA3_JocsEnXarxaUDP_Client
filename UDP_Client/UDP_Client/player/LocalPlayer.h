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
		Vector2 position = Vector2(0.0f, 0.0f);
	};

	unsigned int lastValidatedMovementId = 0;

	std::vector<SentMovement> pendingSentMovements;

	const short maxLives = 3;
	const short maxHealthPoints = 5;

	short currentLives = 3;
	short currentHealthPoints = 5;

	bool defeated = false;
	bool defeatSent = false;

	Vector2 spawnPosition = Vector2(0.0f, 0.0f);

	void Respawn();	

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
	void TrySendMovement();

	void LoseHealthPoint();

	void RecieveDamage(short amount) override;

	bool IsDead() override;

	void SetRespawnPosition(const Vector2& position);
	Vector2 GetRespawnPosition() const { return spawnPosition; }

	inline short GetCurrentLives() const { return currentLives; }
	inline short GetCurrentHealthPoints() const { return currentHealthPoints; }
	inline short GetMaxLives() const { return maxLives; }
	inline short GetMaxHealthPoints() const { return maxHealthPoints; }
	inline bool HasLostMatch() const { return defeated; }
};