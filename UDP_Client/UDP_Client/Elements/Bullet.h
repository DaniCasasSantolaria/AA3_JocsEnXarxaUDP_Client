#pragma once
#include "../ImageObject.h"
#include "../Managers/RenderManager.h"

class Bullet : public ImageObject {
private:
	Vector2 direction;
	float timeHasBeenShooted;
	float marginToDestroy = 50.0f;
	float timeToDestroy = 4.0f;
	float speed = 100.0f;
	unsigned short shooterNetworkId;
	unsigned short damage = 1;
public:
	Bullet(std::string texturepath, Vector2 direction, unsigned short shooterNetworkId, float speed)
		: ImageObject(texturepath, Vector2(0.0f, 0.0f), Vector2(0.0f, 0.0f)), shooterNetworkId(shooterNetworkId), direction(direction), speed(speed) {
		timeHasBeenShooted = TIME.GetElapsedTime();

		sf::Texture* tex = RM->GetTexture(texturepath);
		if (tex != nullptr) {
			sf::Vector2u texSize = tex->getSize();
			transform->size = Vector2(static_cast<float>(texSize.x), static_cast<float>(texSize.y));
		}

		transform->scale = Vector2(direction.x < 0.0f ? -0.5f : 0.5f, 0.5f);
		marginToDestroy = transform->size.x * std::abs(transform->scale.x);
	}
	void Update() override;
	void OnCollisionEnter(Object* other) override;
	inline unsigned short GetShooterNetworkId() const { return shooterNetworkId; }
};