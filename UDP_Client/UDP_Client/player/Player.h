#pragma once
#include <unordered_map>
#include "../ImageObject.h"
#include "../actions/Movement.h"
#include "../actions/Damageable.h"
#include "../actions/Shooting.h"
#include "../Managers/AudioManager.h"
#include "../Elements/Bullet.h"

enum class PlayerState {
	IDLE = 0,
	MOVE = 1,
	ROLL = 2,
	HIT = 3,
	DEATH = 4,
	TAUNT = 5
};

class Player : public ImageObject, public Movement, public Damageable, public Shooting{
protected:
	short maxLifes = 3;
	short life = 5;
	short score = 0;

	bool lookingRight = true;
	bool isGrounded = false;
	float moveSpeed = 300.0f;
	float jumpVelocity = -1050.0f;
	float gravity = 2980.0f;

	bool isTaunting = false;
	float tauntTimer = 0.0f;
	const float tauntDuration = 0.8f;

	bool isHitting = false;
	float hitTimer = 0.0f;
	const float hitDuration = 0.4f;

	PlayerState currentState = PlayerState::IDLE;
public:
	Player() = default;
	Player(std::string texturepath, Vector2 sourceOffset, Vector2 sourceSize, unsigned short numRows, unsigned short numColumns, 
		float frameTime, bool hasToLoop, float shootCooldown, short life, bool hasCollision)
		: ImageObject(texturepath, sourceOffset, sourceSize, numRows, numColumns, frameTime, hasToLoop, hasCollision), Shooting(shootCooldown),
		maxLifes(life) { 
		this->life = maxLifes;

		AnimatedImageRenderer* animatedRenderer = dynamic_cast<AnimatedImageRenderer*>(renderer);

		if (animatedRenderer != nullptr) {
			animatedRenderer->SetAnimation(static_cast<short>(PlayerState::IDLE), 4);
		}
	}

	virtual void Move() = 0;

	void ResolveSolidCollision(Object* other);

	inline void Shoot() override {
		//AUDIO->PlayClip("shoot", 0, 10);
	}

	virtual void Update() = 0;

	void OnCollisionEnter(Object* other) override;
	inline void RecieveDamage(short amount) override {
		life -= amount;
		if (IsDead()) {
			Destroy();
		}
	};

	void ChangeAnimation(PlayerState newState);

	inline void ResetLifes() { life = maxLifes; }
	inline bool IsDead() override { return life <= 0; }
	inline short* GetScore() { return &score; }
	inline void SetScore(int newScore) { score = newScore; }
	inline bool IsLookingRight() const { return lookingRight; }

	void StartTauntAnimation();
	void UpdateTauntAnimation();

	void StartHitAnimation();
	void UpdateHitAnimation();
};