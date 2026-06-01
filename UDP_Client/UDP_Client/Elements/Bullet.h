#pragma once
#include "../ImageObject.h"

class Bullet : public ImageObject {
private:
	Vector2 direction;
	float timeHasBeenShooted;
	float marginToDestroy = 50.0f;
	float timeToDestroy = 4.0f;
	float speed = 100.0f;
	unsigned short shooterNetworkId;
public:
	Bullet(std::string texturepath, Vector2 sourceOffset, Vector2 sourceSize, Vector2 direction, unsigned short shooterNetworkId, float speed)
		: ImageObject(texturepath, sourceOffset, sourceSize), shooterNetworkId(shooterNetworkId), direction(direction), speed(speed) {
		timeHasBeenShooted = TIME.GetElapsedTime();
		transform->scale = Vector2(0.8f, 0.8f);
		marginToDestroy = sourceSize.x * transform->scale.x;
	}
	void Update() override;
	void OnCollisionEnter(Object* other) override;
	inline unsigned short GetShooterNetworkId() const { return shooterNetworkId; }
};