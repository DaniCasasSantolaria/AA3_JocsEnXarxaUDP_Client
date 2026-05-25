#pragma once
#include "Scene.h"
#include "MenuElements/Button.h"
#include "Gameplay/GridBox.h"
#include "player/OnlinePlayer.h"

#define MAX_ROWS 6
#define MAX_COLS 8
#define MAX_PLAYERS 4

class Gameplay : public Scene {
private:
	bool gameFinished = false;
	OnlinePlayer* onlinePlayer = nullptr;

	bool CheckWinCondition(short row, short col);

	void FinishGame();
	bool ShouldFinishGame() const;

public:
	Gameplay() = default;
	void Update() override;
	void OnEnter() override;
	void OnExit() override;
	void Render() override;
};