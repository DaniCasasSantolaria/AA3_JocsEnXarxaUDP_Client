#pragma once
#include "Scene.h"

class MainMenu : public Scene {
private:
	bool hasToPlaySong = true;
public:
	MainMenu() = default;
	void OnEnter() override;
	void OnExit() override;
	void Update() override;
};