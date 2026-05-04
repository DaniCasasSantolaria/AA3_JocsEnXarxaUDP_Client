#pragma once
#include "TextObject.h"

class Score : public TextObject {
private:
	int* score;

public:
	Score(std::string text, sf::Color color, int* score)
	: TextObject(text, color), score(score) {
	}
	~Score() {
		delete score;
	}

	virtual void Update() { TextObject::Update(); }
	virtual void Render() { TextObject::Render(); }
};