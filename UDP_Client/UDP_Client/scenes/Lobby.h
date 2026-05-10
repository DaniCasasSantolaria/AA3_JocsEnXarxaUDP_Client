#pragma once
#include "../Scene.h"
#include "../MenuElements/Button.h"

class Lobby : public Scene
{
private:
    Button* noCompetitiveButton = nullptr;
    Button* competitiveButton = nullptr;
    Button* rankingButton = nullptr;

    void NoCompetitiveAction();
    void CompetitiveAction();
    void RankingSceneAction();

public:
    Lobby() = default;
    void Update() override;
    void OnEnter() override;
    void OnExit() override;
    void Render() override;
};