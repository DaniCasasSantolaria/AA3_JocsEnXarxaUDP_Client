#pragma once
#include "Scene.h"

class SplashScreen : public Scene {
private:
	float currentTime = 0.0f;
	float timeToChangeScene = 4.0f;
public:
	SplashScreen() = default;
	void OnEnter() override;
	void OnExit() override;
	void Update() override;
	void Render() override;
};