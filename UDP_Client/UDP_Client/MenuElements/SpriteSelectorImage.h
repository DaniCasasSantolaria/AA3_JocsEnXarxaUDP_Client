#pragma once
#include "Button.h"

class SpriteSelectorImage : public Button {
private:
	std::string category;
	std::vector<std::string> route;
	std::vector<SpriteSelectorImage*> images;
	ImageObject* imageSelected;
	bool isSelected = false;

	ImageObject* CreateBackgroundObject();
public:
	SpriteSelectorImage(std::string content, sf::Color color, std::vector<std::string> paths, Vector2 sourceOffset, Vector2 sourceSize, std::string category)
		: Button(content, color, sourceOffset, sourceSize), category(category) {
		route = paths;
		textBackground = new ImageObject(paths[0], sourceOffset, sourceSize);
		textBackground->GetTransform()->position = transform->position;
		imageSelected = CreateBackgroundObject();
		imageSelected->GetTransform()->position = transform->position;
		Vector2 topLeft = Vector2(
			transform->position.x - ((transform->size.x * transform->scale.x) / 2.0f),
			transform->position.y - ((transform->size.y * transform->scale.y) / 2.0f));
		Vector2 size = Vector2(
			(sourceSize.x / 2.0f) * transform->scale.x,
			(sourceSize.y / 2.0f) * transform->scale.y);

	/*	topLeft = topLeft - Vector2(50.0f, 50.0f);
		size = size + Vector2(100.0f, 100.0f);*/
		textBackground->GetRigidbody()->AddCollider(new AABB(topLeft, size));
	}

	inline void ChangeScale(Vector2 scale) {
		textBackground->GetRigidbody()->ClearColliders();
		transform->scale = scale;
		Vector2 topLeft = Vector2(
			transform->position.x - ((transform->size.x * transform->scale.x) / 2.0f),
			transform->position.y - ((transform->size.y * transform->scale.y) / 2.0f));
		Vector2 size = Vector2(
			(transform->size.x / 2.0f) * transform->scale.x,
			(transform->size.y / 2.0f) * transform->scale.y);
		textBackground->GetRigidbody()->AddCollider(new AABB(topLeft, size));
	}

	inline void AddOtherImages(std::vector<SpriteSelectorImage*> images) {
		for (int i = 0; i < images.size(); i++) {
			this->images.push_back(images[i]);
		}
	}
	void Update() override;
	inline void Render() override {
		if(imageSelected != nullptr)
			imageSelected->Render();
		if (textBackground != nullptr)
			textBackground->Render();
		Object::Render();
	}
	inline std::vector<std::string> GetRoute() { return route; }
	inline bool GetIsSelected() { return isSelected; }
	inline std::string GetCategory() { return category; }
	void Select();
	void DeSelect();
	inline ImageObject* GetImageSelected() { return imageSelected; }
};