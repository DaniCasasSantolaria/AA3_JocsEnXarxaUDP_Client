#pragma once
#include "../ImageObject.h"

class Bullet : public ImageObject {
private:
	Vector2 direction;
	float timeHasBeenShooted;
	float marginToDestroy = 50.0f;
	float timeToDestroy = 4.0f;
	float speed = 100.0f;
	std::string shooter;
public:
	Bullet(std::string texturepath, Vector2 sourceOffset, Vector2 sourceSize, Vector2 direction, std::string shooter, float speed)
		: ImageObject(texturepath, sourceOffset, sourceSize), shooter(shooter), direction(direction), speed(speed) {
		timeHasBeenShooted = TIME.GetElapsedTime();
		transform->scale = Vector2(0.8f, 0.8f);
		marginToDestroy = sourceSize.x * transform->scale.x;
	}
	void Update() override;
	inline std::string GetShooter() { return shooter; }
};