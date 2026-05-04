#include "SpriteSelectorImage.h"
#include "../Managers/InputManager.h"
#include "../Managers/AudioManager.h"

ImageObject* SpriteSelectorImage::CreateBackgroundObject() {
	ImageObject* o = new ImageObject("resources/azul.png", Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f));
	o->GetTransform()->scale = textBackground->GetTransform()->scale;
	o->GetTransform()->position = transform->position;
	return o;
}

void SpriteSelectorImage::Update() {
	Object::Update();
	if (!Input.GetLeftClick()) {
		isRightMouseDown = false;
	}
	if (textBackground->GetRigidbody()->CheckOverlappingPoint(Vector2((float)Input.GetMouseX(), (float)Input.GetMouseY()))) {
		if (!hasPlayedSound) {
			AUDIO->PlayClip("click", 0, 20);
			hasPlayedSound = true;
		}
		if (Input.GetLeftClick()) {
			if (!isRightMouseDown) {
				isSelected = true;
				isRightMouseDown = true;
			}
		}
	}
	else {
		hasPlayedSound = false;
	}
	isSelected = false;
}

void SpriteSelectorImage::Select() {
	isSelected = true;
	imageSelected->GetTransform()->size = textBackground->GetTransform()->size + Vector2(10.0f, 10.0f);
}

void SpriteSelectorImage::DeSelect() {
	isSelected = false;
	imageSelected->GetTransform()->size = Vector2(1.0f, 1.0f);
}