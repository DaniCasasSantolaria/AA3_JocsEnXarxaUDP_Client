#pragma once
#include "Player.h"
#include <vector>

class OnlinePlayer : public Player {
private:
	struct MovementReceive {
		unsigned int movementId = 0;
		Vector2 position = Vector2(0.0f, 0.0f);
	};

	std::vector<MovementReceive> pendingReceivedMovements;

	unsigned int lastAppliedMovementId = 0;

	float interpolationFactor = 0.18f;
	float snapDistance = 2.0f;
	float animationMoveThreshold = 0.5f;

	static Vector2 Lerp(const Vector2& a, const Vector2& b, float t);
	static float DistanceSquared(const Vector2& a, const Vector2& b);

	void UpdateAnimationFromMovement(const Vector2& oldPosition, const Vector2& newPosition);

public:
	OnlinePlayer() = default;

	OnlinePlayer(std::string texturepath, Vector2 sourceOffset, Vector2 sourceSize, unsigned short numRows, unsigned short numColumns, float frameTime,
		bool hasToLoop,	float shootCooldown, short life)
		: Player(texturepath, sourceOffset, sourceSize, numRows, numColumns, frameTime, hasToLoop, shootCooldown, life) {
		this->GetRigidbody()->SetGravity(0.0f);
		this->GetRigidbody()->SetVelocity(Vector2(0.0f, 0.0f));
	}

	void Move() override;
	void Update() override;

	void AddServerMovement(unsigned int movementId, const Vector2& serverPosition);

	void OnCollisionEnter(Object* other) override {
		// El jugador online no colisiona con nada localmente
	}
};