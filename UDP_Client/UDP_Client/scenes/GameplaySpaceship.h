#pragma once
#include "../Gameplay.h"

class GameplaySpaceship : public Gameplay {
public:
	GameplaySpaceship() = default;
	void Update() override;
	void OnEnter() override;

	/*int GetHighScore() override;
	void SaveHighScore(const std::string& playerName, int score) override;*/
};