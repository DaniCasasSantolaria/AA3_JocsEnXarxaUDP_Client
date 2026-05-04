#include "Bullet.h"
#include "../Managers/RenderManager.h"

void Bullet::Update() {
	ImageObject::Update();
	physics->SetVelocity(direction * speed);
	if (TIME.GetElapsedTime() - timeHasBeenShooted > timeToDestroy 
		|| transform->position.x > RM->WINDOW_WIDTH + marginToDestroy || transform->position.x < 0 - marginToDestroy
		|| transform->position.y > RM->WINDOW_HEIGHT + marginToDestroy || transform->position.y < 0 - marginToDestroy)
		Destroy();
}