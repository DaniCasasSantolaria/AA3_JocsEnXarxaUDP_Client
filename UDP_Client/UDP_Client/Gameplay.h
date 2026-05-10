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
	short myPlayerID = -1;

	float currentMoveTime = 0.0f;
	float maxTimeToMove = 20.0f;

	short currentPlayerIDTurn = 0;

	unsigned short piecesPlaced = 0;

	unsigned short maxPlayers = 0;

	bool gameFinished = false;

	void OnGridBoxPressed(short row, short col);
	bool IsInside(short row, short col) const;
	short CountDirection(short row, short col, short dRow, short dCol, short player) const;
	bool CheckWinCondition(short row, short col);
	void ChangeGridBox(short row, short col, short playerID);

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