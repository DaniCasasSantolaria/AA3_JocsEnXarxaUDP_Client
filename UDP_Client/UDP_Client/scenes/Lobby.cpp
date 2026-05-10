#include "Lobby.h"
#include "../ImageObject.h"
#include "../Spawner.h"
#include "../Managers/SceneManager.h"
#include <iostream>
#include "../Managers/InputManager.h"
#include "../Managers/PacketManager.h"

void Lobby::OnEnter() {
	// Fondo de pantalla
	Object* background = new ImageObject("resources/Login/Fondo.png", Vector2{ 0, 0 }, Vector2{ 1536, 1024 });
	background->GetTransform()->position = { RM->WINDOW_WIDTH / 2.0f, RM->WINDOW_HEIGHT / 2.0f };
	background->GetTransform()->scale = { 1.0f, 1.0f };
	SPAWN.SpawnObject(background);

	// Botón para crear sala
	noCompetitiveButton = new Button("", sf::Color{ 18, 113, 230 }, Vector2(0.0f, 0.0f), Vector2(660.0f, 140.0f), "resources/Lobby/create-button.png");
	noCompetitiveButton->SetAction([this]() {
		NoCompetitiveAction();
		});
	noCompetitiveButton->GetTransform()->scale = Vector2(1.0f, 1.0f);
	noCompetitiveButton->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f, RM->WINDOW_HEIGHT / 2.0f);
	noCompetitiveButton->GetBackground()->GetTransform()->scale = Vector2(0.5f, 0.5f);
	noCompetitiveButton->GetBackground()->GetTransform()->position = noCompetitiveButton->GetTransform()->position;
	SPAWN.SpawnObject(noCompetitiveButton->GetBackground());
	SPAWN.SpawnObject(noCompetitiveButton);

	// Botón para unirse a sala
	competitiveButton = new Button("", sf::Color{ 18, 113, 230 }, Vector2(0.0f, 0.0f), Vector2(660.0f, 140.0f), "resources/Lobby/join-button.png");
	competitiveButton->SetAction([this]() {
		CompetitiveAction();
		});
	competitiveButton->GetTransform()->scale = Vector2(1.0f, 1.0f);
	competitiveButton->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f, RM->WINDOW_HEIGHT / 2.0f + 100);
	competitiveButton->GetBackground()->GetTransform()->scale = Vector2(0.5f, 0.5f);
	competitiveButton->GetBackground()->GetTransform()->position = competitiveButton->GetTransform()->position;
	SPAWN.SpawnObject(competitiveButton->GetBackground());
	SPAWN.SpawnObject(competitiveButton);

	// Botón para ver ranking
	rankingButton = new Button("", sf::Color{ 18, 113, 230 }, Vector2(0.0f, 0.0f), Vector2(660.0f, 140.0f), "resources/Lobby/ranking-button.png");
	rankingButton->SetAction([this]() {
		std::cout << "MOVED TO RANKING SCREEN: " << std::endl;
		RankingSceneAction();
		});
	rankingButton->GetTransform()->scale = Vector2(1.0f, 1.0f);
	rankingButton->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f, RM->WINDOW_HEIGHT / 2.0f + 200);
	rankingButton->GetBackground()->GetTransform()->scale = Vector2(1.0f, 1.0f);
	rankingButton->GetBackground()->GetTransform()->position = rankingButton->GetTransform()->position;
	SPAWN.SpawnObject(rankingButton->GetBackground());
	SPAWN.SpawnObject(rankingButton);

	Input.BeginTextInput();

	// Solicitar datos del ranking al servidor
	PM->RankingRequest();
}

void Lobby::OnExit() {
	Scene::OnExit();
	Input.EndTextInput();
}

void Lobby::Update() {
	Scene::Update();

	// Ocultar botones cuando se entra a una sala o se crea
	if (PM->hideAllButtons)
	{
		noCompetitiveButton->GetBackground()->Destroy();
		noCompetitiveButton->Destroy();
		noCompetitiveButton = nullptr;

		competitiveButton->GetBackground()->Destroy();
		competitiveButton->Destroy();
		competitiveButton = nullptr;

		rankingButton->GetBackground()->Destroy();
		rankingButton->Destroy();
		rankingButton = nullptr;

		PM->hideAllButtons = false;
	}
}

void Lobby::Render() {
	Scene::Render();
}

void Lobby::NoCompetitiveAction() {
	PM->SendMatchmakeRequest(NON_COMPETITIVE);
}

void Lobby::CompetitiveAction() {
	PM->SendMatchmakeRequest(COMPETITIVE);
}

void Lobby::RankingSceneAction()
{
	// Solicitar ranking y cambiar a escena de ranking
	PM->RankingRequest();
	SM.SetNextScene("Ranking Screen");
}