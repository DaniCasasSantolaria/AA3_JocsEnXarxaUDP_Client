#pragma once
#include "Scene.h"
#include "MenuElements/Button.h"
#include "Gameplay/GridBox.h"
#include "player/OnlinePlayer.h"
#include "player/LocalPlayer.h"

class Gameplay : public Scene {
private:
	bool gameFinished = false;

	OnlinePlayer* onlinePlayer = nullptr;
	LocalPlayer* localPlayer = nullptr;

	TextObject* resultText = nullptr;
	float resultTimer = 0.0f;
	const float RESULT_TIME = 3.0f;

	void PrintHealthDebug(const char* reason);

public:
	Gameplay() = default;
	void Update() override;
	void OnEnter() override;
	void OnExit() override;
	void Render() override;
};