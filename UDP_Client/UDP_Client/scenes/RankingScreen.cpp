#include "RankingScreen.h"
#include "../ImageObject.h"
#include "../Spawner.h"
#include "../Managers/SceneManager.h"
#include "../Managers/InputManager.h"
#include "../Managers/PacketManager.h"

#include <iostream>

// Fondo de pantalla
void SpawnBackground() {
	Object* background = new ImageObject("resources/Login/Fondo_clean.png", Vector2{ 0, 0 }, Vector2{ 1536, 1024 });
	background->GetTransform()->position = { RM->WINDOW_WIDTH / 2.0f, RM->WINDOW_HEIGHT / 2.0f };
	background->GetTransform()->scale = { 1.0f, 1.0f };
	SPAWN.SpawnObject(background);
}

// Título del ranking
void SpawnRankinTitle() {
	Object* rankingName = new TextObject("Ranking", sf::Color{ 255, 255, 255 });
	rankingName->GetTransform()->scale = Vector2(3.0f, 3.0f);
	rankingName->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f, 100.0f);
	SPAWN.SpawnObject(rankingName);
}

// Puntuaciones del ranking
void SpawnRankingScores(const std::vector<PlayerScore>& scores) {
	Object* ranking = new TextObject("Top 10", sf::Color{ 255, 255, 255 });
	ranking->GetTransform()->scale = Vector2(1.75f, 1.75f);
	ranking->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f, 220.0f);
	SPAWN.SpawnObject(ranking);

	for (size_t i = 0; i < scores.size(); ++i) {
		// Formatear puntuación con ceros a la izquierda
		std::string formattedScore = std::to_string(scores[i].score);
		while (formattedScore.length() < 3) {
			formattedScore = "0" + formattedScore;
		}

		// Crear texto con formato : posición.nombre puntuación
		Object* playerScore = new TextObject(std::to_string(scores[i].position) + ". " + scores[i].name + " " + formattedScore, sf::Color{255, 255, 255});
		playerScore->GetTransform()->scale = Vector2(1.0f, 1.0f);
		playerScore->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f, 250.0f + ((1 + i) * 35.0f));
		SPAWN.SpawnObject(playerScore);
	}
}

// Botón para volver al lobby
void SpawnBack() {
	Button* backButton = new Button("", sf::Color{ 18, 113, 230 }, Vector2(0.0f, 0.0f), Vector2(660.0f, 140.0f), "resources/Ranking/back-button.png");
	backButton->SetAction([]() {
		SM.SetNextScene("Lobby");
		});
	backButton->GetTransform()->scale = Vector2(1.0f, 1.0f);
	backButton->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f, RM->WINDOW_HEIGHT / 2.0f + 300);
	backButton->GetBackground()->GetTransform()->scale = Vector2(0.5f, 0.5f);
	backButton->GetBackground()->GetTransform()->position = backButton->GetTransform()->position;
	SPAWN.SpawnObject(backButton->GetBackground());
	SPAWN.SpawnObject(backButton);
}

void RankingScreen::OnEnter()
{
	SpawnBackground();
	SpawnRankinTitle();
	LoadScore();
	SpawnRankingScores(scores);
	SpawnBack();
}

void RankingScreen::OnExit()
{
	// Solicitar actualización del ranking al salir
	PM->RankingRequest();
	Scene::OnExit();
}

void RankingScreen::Update()
{
	Scene::Update();
}

// Carga las puntuaciones y añadirlas a la lista de scores desde PacketManager
void RankingScreen::LoadScore() {
	scores.clear();

	// Copiar puntuaciones desde PacketManager
	for (const PacketManager::PlayerScore& pmScore : PM->ranking) {
		PlayerScore ps;
		ps.name = pmScore.name;
		ps.score = pmScore.score;
		ps.position = pmScore.position;
		scores.push_back(ps);
		std::cout << "Loaded score: " << ps.name << " - " << ps.score << " - " << ps.position << std::endl;
	}
}