#include "Player.h"

void Player::OnCollisionEnter(Object* other) {
	if (Bullet* bullet = dynamic_cast<Bullet*>(other)) {
		if (bullet->GetShooter() == "Enemy") {
			RecieveDamage(1);
			AUDIO->PlayClip("hit_enemy", 0, 80);
			other->Destroy();
		}
	}
}