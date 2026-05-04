#include "VolumeButton.h"
#include "Button.h"
#include "../Managers/InputManager.h"
#include <iostream>
#include "../Managers/AudioManager.h"
#include "../Spawner.h"

Object* VolumeButton::CreateNoVolume() {
	Object* o = new ImageObject("resources/noVolume.png", Vector2(0.0f, 0.0f), textBackground->GetTransform()->size);
	o->GetTransform()->position = transform->position;
	Vector2 topLeft2 = Vector2(o->GetTransform()->position.x - ((o->GetTransform()->size.x * o->GetTransform()->scale.x) / 2.0f),
		o->GetTransform()->position.y - ((o->GetTransform()->size.y * o->GetTransform()->scale.y) / 2.0f));
	Vector2 size2 = Vector2((o->GetTransform()->size.x / 2.0f, o->GetTransform()->size.x / 2.0f) * o->GetTransform()->scale.x,
		(o->GetTransform()->size.y / 2.0f, o->GetTransform()->size.y / 2.0f) * o->GetTransform()->scale.y);
	o->GetRigidbody()->AddCollider(new AABB(topLeft2, size2));
	o->GetTransform()->scale = Vector2(0.3f, 0.3f);
	return o;
}

Object* VolumeButton::CreateVolume() {
	Object* o = new ImageObject("resources/volume.png", Vector2(0.0f, 0.0f), textBackground->GetTransform()->size);
	o->GetTransform()->position = transform->position;
	Vector2 topLeft2 = Vector2(o->GetTransform()->position.x - ((o->GetTransform()->size.x * o->GetTransform()->scale.x) / 2.0f),
		o->GetTransform()->position.y - ((o->GetTransform()->size.y * o->GetTransform()->scale.y) / 2.0f));
	Vector2 size2 = Vector2((o->GetTransform()->size.x / 2.0f, o->GetTransform()->size.x / 2.0f) * o->GetTransform()->scale.x,
		(o->GetTransform()->size.y / 2.0f, o->GetTransform()->size.y / 2.0f) * o->GetTransform()->scale.y);
	o->GetRigidbody()->AddCollider(new AABB(topLeft2, size2));
	o->GetTransform()->scale = Vector2(0.3f, 0.3f);
	return o;
}

void VolumeButton::Update() {
	Object::Update();
	if (!Input.GetLeftClick()) {
		isRightMouseDown = false;
	}
	if (textBackground->GetRigidbody()->CheckOverlappingPoint(Vector2(Input.GetMouseX(), Input.GetMouseY())) &&
		Input.GetLeftClick()) {
		if (!isRightMouseDown) {
			if (!AUDIO->GetMuted()) {
				textBackground->Destroy();
				textBackground = CreateNoVolume();
				SPAWN.SpawnObject(textBackground);
				AUDIO->Mute();
			}
			else {
				textBackground->Destroy();
				textBackground = CreateVolume();
				SPAWN.SpawnObject(textBackground);
				AUDIO->Unmute();
			}
			isRightMouseDown = true;
		}
	}
}