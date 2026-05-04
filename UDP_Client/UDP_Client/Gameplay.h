#pragma once
#include "Scene.h"
#include "MenuElements/Button.h"
#include "Gameplay/GridBox.h"

#define MAX_ROWS 6
#define MAX_COLS 8
#define MAX_PLAYERS 4

class Gameplay : public Scene {
private:
	GridBox* grid[MAX_ROWS][MAX_COLS];
	std::string usernamesByIndex[MAX_PLAYERS];
	TextObject* playerInfoText[MAX_PLAYERS];

	int scoreByIndex[MAX_PLAYERS];
	TextObject* scoreInfoText[MAX_PLAYERS];
	int myPlayerID = -1;

	float currentMoveTime = 0.0f;
	float maxTimeToMove = 20.0f;

	int currentPlayerIDTurn = 0;

	int piecesPlaced = 0;

	int maxPlayers = 0;

	bool gameFinished = false;

	void OnGridBoxPressed(int row, int col);
	bool IsInside(int row, int col) const;
	int CountDirection(int row, int col, int dRow, int dCol, int player) const;
	bool CheckWinCondition(int row, int col);
	void ChangeGridBox(int row, int col, int playerID);

	inline bool IsMyTurn() const { return myPlayerID == currentPlayerIDTurn; }
	void ChangeTurn();

	void FinishGame();
	bool ShouldFinishGame() const;

public:
	Gameplay() = default;
	void Update() override;
	void OnEnter() override;
	void OnExit() override;
	void Render() override;
};