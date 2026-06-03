#pragma once
#include "Button.h"

class VolumeButton : public Button {
public:
	VolumeButton(std::string content, sf::Color color, std::string path, Vector2 sourceOffset, Vector2 sourceSize)
		: Button(content, color, sourceOffset, sourceSize) {
		textBackground = new ImageObject(path, sourceOffset, sourceSize);
		textBackground->GetTransform()->position = transform->position;
		Vector2 topLeft = Vector2(textBackground->GetTransform()->position.x - ((textBackground->GetTransform()->size.x * textBackground->GetTransform()->scale.x) / 2.0f),
			textBackground->GetTransform()->position.y - ((textBackground->GetTransform()->size.y * textBackground->GetTransform()->scale.y) / 2.0f));
		Vector2 size = Vector2((sourceSize.x / 2.0f, sourceSize.x / 2.0f) * textBackground->GetTransform()->scale.x,
			(sourceSize.y / 2.0f, sourceSize.y / 2.0f) * textBackground->GetTransform()->scale.y);
		textBackground->GetRigidbody()->AddCollider(new AABB(topLeft, size));
	}

	Object* CreateNoVolume();
	Object* CreateVolume();

	inline void SetBackground(Object* o) { textBackground = o; }

	void Update() override;
	inline void Render() override {
		if(textBackground != nullptr)
			textBackground->Render();
		Object::Render();
	}
};