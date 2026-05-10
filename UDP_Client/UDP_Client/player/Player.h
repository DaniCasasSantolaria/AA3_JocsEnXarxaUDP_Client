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
	DEATH = 4
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

	PlayerState currentState = PlayerState::IDLE;
	Vector2 currentDirection = Vector2(0, 0);
public:
	Player() = default;
	Player(std::string texturepath, Vector2 sourceOffset, Vector2 sourceSize, int numRows, int numColumns, float frameTime, bool hasToLoop, float shootCooldown, int life)
		: ImageObject(texturepath, sourceOffset, sourceSize, numRows, numColumns, frameTime, hasToLoop), Shooting(shootCooldown),
		maxLifes(life) { 
		this->life = maxLifes;
		this->GetRigidbody()->SetGravity(gravity);

		AnimatedImageRenderer* animatedRenderer = dynamic_cast<AnimatedImageRenderer*>(renderer);

		if (animatedRenderer != nullptr) {
			animatedRenderer->SetAnimation(static_cast<short>(PlayerState::IDLE), 4);
		}
	}

	void Move();

	void ResolveSolidCollision(Object* other);

	inline void Shoot() override {
		//AUDIO->PlayClip("shoot", 0, 10);
	}

	inline void Update() override {
		Move();
		isGrounded = false;

		ImageObject::Update();

		Shoot();
	}

	void OnCollisionEnter(Object* other) override;
	inline void RecieveDamage(int amount) override {
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
};