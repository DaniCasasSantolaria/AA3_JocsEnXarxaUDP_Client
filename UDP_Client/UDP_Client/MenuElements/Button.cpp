#include "Button.h"
#include "../Managers/InputManager.h"
#include "../Managers/SceneManager.h"
#include <iostream>
#include "../Managers/AudioManager.h"

void Button::Update() {
	Object::Update();
	if (!Input.GetLeftClick()) {
		isRightMouseDown = false;
	}
	if (textBackground->GetRigidbody()->CheckOverlappingPoint(Vector2((float)Input.GetMouseX(), (float)Input.GetMouseY()))) {
		if (!hasPlayedSound) {
			AUDIO->PlayClip("hover", 0, 20);
			hasPlayedSound = true;
		}
		if (Input.GetLeftClick()) {
			if (!isRightMouseDown) {
				DoAction();
				isRightMouseDown = true;
			}
		}
	}
	else {
		hasPlayedSound = false;
	}
}

void Button::Render() {
	textBackground->Render();
	Object::Render();
}