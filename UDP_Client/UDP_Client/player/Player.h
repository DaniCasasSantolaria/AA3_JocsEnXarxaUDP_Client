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
	SHOOT = 2,
	DEATH = 3
};

class Player : public ImageObject, public Movement, public Damageable, public Shooting{
protected:
	int maxLifes;
	int life;
	int score = 0;

	PlayerState currentState = PlayerState::IDLE;
	std::unordered_map<int, Vector2> movementForces = {
	{0, Vector2(0, 0)},         // Est� quiet
	{1, Vector2(5500, 0)},        // Dreta
	{2, Vector2(0, -5500)},       // Adalt
	{3, Vector2(-5500, 0)},       // Esquerra
	{4, Vector2(0, 5500)},        // Abaix
	};
	Vector2 currentDirection = Vector2(0, 0);
public:
	Player() = default;
	Player(std::string texturepath, Vector2 sourceOffset, Vector2 sourceSize, float shootCooldown, int life)
		: ImageObject(texturepath, sourceOffset, sourceSize), Shooting(shootCooldown),
		maxLifes(life) { 
		this->life = maxLifes;
	}

	virtual void Move() = 0;
	inline virtual void Shoot() override {
		AUDIO->PlayClip("shoot", 0, 10);
	}
	inline virtual void Update() override { 
		ImageObject::Update();
		Move(); 
		Shoot();
	}
	virtual void OnCollisionEnter(Object* other) override;
	inline void RecieveDamage(int amount) override {
		life -= amount;
		if (IsDead()) {
			Destroy();
		}
	};
	inline void ResetLifes() { life = maxLifes; }
	inline bool IsDead() override { return life <= 0; }
	inline int* GetScore() { return &score; }
	inline void SetScore(int newScore) { score = newScore; }
};