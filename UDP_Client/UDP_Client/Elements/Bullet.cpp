#include "Bullet.h"
#include "../Managers/RenderManager.h"
#include "../Managers/PacketManager.h"
#include "../player/Player.h"
#include "../player/LocalPlayer.h"

void Bullet::Update() {
	ImageObject::Update();
	physics->SetVelocity(direction * speed);
	if (TIME.GetElapsedTime() - timeHasBeenShooted > timeToDestroy
		|| transform->position.x > RM->WINDOW_WIDTH + marginToDestroy || transform->position.x < 0 - marginToDestroy
		|| transform->position.y > RM->WINDOW_HEIGHT + marginToDestroy || transform->position.y < 0 - marginToDestroy)
		Destroy();
}

void Bullet::OnCollisionEnter(Object* other) {

    if (dynamic_cast<Bullet*>(other) != nullptr) return;

    LocalPlayer* localPlayer = dynamic_cast<LocalPlayer*>(other);

    if (localPlayer != nullptr) {

        if (shooterNetworkId == PM->GetMyIndex()) {
            return;
        }

        localPlayer->RecieveDamage(damage);

        Destroy();
        return;
    }

    Player* player = dynamic_cast<Player*>(other);

    if (player != nullptr) {
        if (shooterNetworkId == PM->GetMyIndex()) {
            Destroy();
            return;
        }

        return;
    }

    if (dynamic_cast<ImageObject*>(other)) {
        Destroy();
        return;
    }
}