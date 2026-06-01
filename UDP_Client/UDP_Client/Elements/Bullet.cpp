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
	if (dynamic_cast<Player*>(other)) {
		if (dynamic_cast<LocalPlayer*>(other) && shooterNetworkId == PM->GetMyIndex()) return;
		if (LocalPlayer* lp = dynamic_cast<LocalPlayer*>(other)) {
			lp->RecieveDamage(1);
		}
		Destroy();
		return;
	}
	if (dynamic_cast<ImageObject*>(other)) Destroy();
}