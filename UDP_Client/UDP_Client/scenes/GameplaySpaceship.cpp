#include "GameplaySpaceship.h"
#include "../Spawner.h"
#include "../Managers/InputManager.h"
#include "../Managers/SceneManager.h"

#include <algorithm>

void GameplaySpaceship::Update() {
	Gameplay::Update();
}

void GameplaySpaceship::OnEnter() {
	/*Object* background = new ImageObject(SPRITES.GetSelectedBackground(0), Vector2(0.0f, 0.0f), Vector2(5000.0f, 3000.0f));
	background->GetTransform()->scale = Vector2(1.0f, 1.0f);
	background->GetTransform()->size = Vector2(RM->WINDOW_WIDTH, RM->WINDOW_HEIGHT);
	//background->GetTransform()->scale = Vector2(1.85f, (background->GetTransform()->size.y / background->GetTransform()->size.x) * 1.85f);
	background->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f, RM->WINDOW_HEIGHT / 2.0f - 150.0f);
	SPAWN.SpawnObject(background);
	*/

	/*Gameplay::OnEnter();

	Spaceship* spaceship = new Spaceship("resources/Spaceship.png", Vector2(0.0f, 0.0f), Vector2(48.0f, 48.0f), 1.0f, 3);
	spaceship->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f, RM->WINDOW_HEIGHT - 150.0f);
	player = spaceship;
	SPAWN.SpawnObject(spaceship);

	
	float patternTime = 24.0f;
	cooldownToSpawnEnemies = patternTime;

	AUDIO->PlaySong("spaceship_gameplay", 20);*/
}

//int GameplaySpaceship::GetHighScore() {
//	std::vector<PlayerScore> scores;
//	const std::string filePath = "resources/rankings/space_invaders.dat";
//
//	return 0;
//}
//
//void GameplaySpaceship::SaveHighScore(const std::string& playerName, int score) {
//    std::vector<PlayerScore> scores;
//    const std::string filePath = "resources/rankings/space_invaders.dat";
//
//	std::string scoreText = std::to_string(score);
//	while (scoreText.length() < 6) {
//		scoreText = "0" + scoreText;
//	}
//	score = std::stoi(scoreText);
//
//    PlayerScore newScore = { playerName.substr(0, 5), score };
//    auto it = std::find_if(scores.begin(), scores.end(), [&](const PlayerScore& ps) {
//        return ps.score == score;
//        });
//    if (it != scores.end()) {
//        scores.insert(it, newScore);
//    }
//    else {
//        scores.push_back(newScore);
//    }
//
//    std::sort(scores.begin(), scores.end(), [](const PlayerScore& a, const PlayerScore& b) {
//        return a.score > b.score;
//        });
//
//    if (scores.size() > 10) {
//        scores.resize(10);
//    }
//}

