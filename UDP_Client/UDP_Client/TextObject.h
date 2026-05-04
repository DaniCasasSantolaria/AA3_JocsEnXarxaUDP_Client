#pragma once
#include "Object.h"
#include "Renderer/TextRenderer.h"

class TextObject : public Object {
private:
	TextRenderer* tr;
public:
	TextObject(std::string text, sf::Color color)
		: Object() {
		tr = new TextRenderer(transform, text);
		renderer = tr;
		tr->SetColor(color);
	}
	inline void SetText(std::string text) {
		tr->SetText(text);
	}
	virtual void Update() { Object::Update(); }
	virtual void Render() { Object::Render(); }

	~TextObject() {
		delete tr;
	}
};