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

	// Texto ID de sala
	idLobbyTextButton = new Button("", sf::Color{ 18, 113, 230 }, Vector2(0.0f, 0.0f), Vector2(16.0f, 16.0f), "resources/Lobby/lobby-button.png");
	idLobbyTextButton->SetAction([this]() {
		StartEditingIdLobby();
		});
	idLobbyTextButton->GetTransform()->scale = Vector2(1.0f, 1.0f);
	idLobbyTextButton->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f, RM->WINDOW_HEIGHT / 2.0f - 100.0f);
	idLobbyTextButton->GetBackground()->GetTransform()->scale = Vector2(32.0f, 6.0f);
	idLobbyTextButton->GetBackground()->GetTransform()->position = idLobbyTextButton->GetTransform()->position;
	SPAWN.SpawnObject(idLobbyTextButton->GetBackground());
	SPAWN.SpawnObject(idLobbyTextButton);

	// Botón para crear sala
	createButton = new Button("", sf::Color{ 18, 113, 230 }, Vector2(0.0f, 0.0f), Vector2(16.0f, 16.0f), "resources/Lobby/create-button.png");
	createButton->SetAction([this]() {
		std::cout << "CREATE LOBBY: " << idLobby << std::endl;
		CreateAction();
		});
	createButton->GetTransform()->scale = Vector2(1.0f, 1.0f);
	createButton->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f, RM->WINDOW_HEIGHT / 2.0f);
	createButton->GetBackground()->GetTransform()->scale = Vector2(25.0f, 6.0f);
	createButton->GetBackground()->GetTransform()->position = createButton->GetTransform()->position;
	SPAWN.SpawnObject(createButton->GetBackground());
	SPAWN.SpawnObject(createButton);

	// Botón para unirse a sala
	joinButton = new Button("", sf::Color{ 18, 113, 230 }, Vector2(0.0f, 0.0f), Vector2(16.0f, 16.0f), "resources/Lobby/join-button.png");
	joinButton->SetAction([this]() {
		std::cout << "JOINED LOBBY: " << idLobby << std::endl;
		JoinAction();
		});
	joinButton->GetTransform()->scale = Vector2(1.0f, 1.0f);
	joinButton->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f, RM->WINDOW_HEIGHT / 2.0f + 100);
	joinButton->GetBackground()->GetTransform()->scale = Vector2(25.0f, 6.0f);
	joinButton->GetBackground()->GetTransform()->position = joinButton->GetTransform()->position;
	SPAWN.SpawnObject(joinButton->GetBackground());
	SPAWN.SpawnObject(joinButton);

	// Botón para ver ranking
	rankingButton = new Button("", sf::Color{ 18, 113, 230 }, Vector2(0.0f, 0.0f), Vector2(16.0f, 16.0f), "resources/Lobby/ranking-button.png");
	rankingButton->SetAction([this]() {
		std::cout << "MOVED TO RANKING SCREEN: " << idLobby << std::endl;
		RankingSceneAction();
		});
	rankingButton->GetTransform()->scale = Vector2(1.0f, 1.0f);
	rankingButton->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f, RM->WINDOW_HEIGHT / 2.0f + 200);
	rankingButton->GetBackground()->GetTransform()->scale = Vector2(25.0f, 6.0f);
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

	if (activeField == ActiveField::IdLobby) {
		idLobby = Input.GetInputText();
	}

	// Actualizar texto
	if (idLobbyTextButton) {
		idLobbyTextButton->SetText(idLobby.empty() ? "ID Lobby" : idLobby);
	}

	// Ocultar botones cuando se entra a una sala o se crea
	if (PM->hideAllButtons)
	{
		createButton->GetBackground()->Destroy();
		createButton->Destroy();
		
		joinButton->GetBackground()->Destroy();
		joinButton->Destroy();
		
		rankingButton->GetBackground()->Destroy();
		rankingButton->Destroy();

		PM->hideAllButtons = false;
	}
}

void Lobby::Render() {
	Scene::Render();
}

void Lobby::StartEditingIdLobby() {
	if (activeField == ActiveField::IdLobby) {
		idLobby = Input.GetInputText();
	}

	activeField = ActiveField::IdLobby;
	Input.SetInputText(idLobby);
}

void Lobby::CreateAction() {
	PM->CreateActionRequest(idLobby);
}

void Lobby::JoinAction() {
	PM->JoinActionRequest(idLobby);
}

void Lobby::RankingSceneAction()
{
	// Solicitar ranking y cambiar a escena de ranking
	PM->RankingRequest();
	SM.SetNextScene("Ranking Screen");
}