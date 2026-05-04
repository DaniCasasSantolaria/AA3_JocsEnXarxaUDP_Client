#pragma once
#include "../TextObject.h"
#include "../ImageObject.h"
#include "../Gameplay.h"
#include <functional>

class Button : public TextObject {
protected:
	std::function<void()> action;
	Object* textBackground;
	bool isRightMouseDown = false;
	bool hasPlayedSound = false;
public:
	Button(std::string content, sf::Color color, Vector2 sourceOffset, Vector2 sourceSize)
		: TextObject(content, color) {
		textBackground = new ImageObject("resources/pixil-frame-0.png", sourceOffset, sourceSize);
		textBackground->GetTransform()->scale = Vector2(sourceSize.x / 1.25f, sourceSize.y / 2.5f);
		textBackground->GetTransform()->position = transform->position;
		
		Vector2 topLeft = Vector2(textBackground->GetTransform()->position.x - ((textBackground->GetTransform()->size.x * textBackground->GetTransform()->scale.x) / 2.0f),
			textBackground->GetTransform()->position.y - ((textBackground->GetTransform()->size.y * textBackground->GetTransform()->scale.y) / 2.0f));
		Vector2 size = Vector2((sourceSize.x / 2.0f, sourceSize.x / 2.0f) * textBackground->GetTransform()->scale.x, 
			(sourceSize.y / 2.0f, sourceSize.y / 2.0f) * textBackground->GetTransform()->scale.y);
		textBackground->GetRigidbody()->AddCollider(new AABB(topLeft, size));
	}

	Button(std::string content, sf::Color color, Vector2 sourceOffset, Vector2 sourceSize, std::string imageSource)
		: TextObject(content, color) {
		textBackground = new ImageObject(imageSource, sourceOffset, sourceSize);
		textBackground->GetTransform()->scale = Vector2(sourceSize.x / 1.25f, sourceSize.y / 2.5f);
		textBackground->GetTransform()->position = transform->position;

		Vector2 topLeft = Vector2(textBackground->GetTransform()->position.x - ((textBackground->GetTransform()->size.x * textBackground->GetTransform()->scale.x) / 2.0f),
			textBackground->GetTransform()->position.y - ((textBackground->GetTransform()->size.y * textBackground->GetTransform()->scale.y) / 2.0f));
		Vector2 size = Vector2((sourceSize.x / 2.0f, sourceSize.x / 2.0f) * textBackground->GetTransform()->scale.x,
			(sourceSize.y / 2.0f, sourceSize.y / 2.0f) * textBackground->GetTransform()->scale.y);
		textBackground->GetRigidbody()->AddCollider(new AABB(topLeft, size));
	}
	inline void SetAction(std::function<void()> a) { action = a; }
	inline Object* GetBackground() const { return textBackground; }
	inline void SetBackground(Object* background) { textBackground = background; }
	virtual void Update() override;
	virtual void Render() override;
	inline void DoAction() { action(); }
	~Button() {
		delete textBackground;
	}
};