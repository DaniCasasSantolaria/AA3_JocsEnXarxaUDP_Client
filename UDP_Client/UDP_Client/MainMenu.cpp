#include "MainMenu.h"
#include "MenuElements/Button.h"
#include "Spawner.h"
#include "Managers/RenderManager.h"
#include "Managers/AudioManager.h"
#include "MenuElements/VolumeButton.h"
#include "Managers/InputManager.h"
#include "Managers/SceneManager.h"

void SpawnTitle() {
	Object* gameName = new TextObject("Games Designer", sf::Color{ 255, 255, 255 });
	gameName->GetTransform()->scale = Vector2(3.0f, 3.0f);
	gameName->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f, 100.0f);
	SPAWN.SpawnObject(gameName);
}

void SpawnPlayButtons(bool &hasToPlaySong) {
	Button* play1 = new Button("Invaders", sf::Color{ 18, 113, 230 }, Vector2(0.0f, 0.0f), Vector2(16.0f, 16.0f));
	play1->SetAction([&hasToPlaySong]() {
		hasToPlaySong = true;
		SM.SetNextScene("Gameplay Spaceship");
		});
	play1->GetTransform()->scale = Vector2(1.75f, 1.75f);
	play1->GetTransform()->position = Vector2((RM->WINDOW_WIDTH / 2.0f) - 250.0f, 250.0f);
	play1->GetBackground()->GetTransform()->position = play1->GetTransform()->position;
	SPAWN.SpawnObject(play1->GetBackground());
	SPAWN.SpawnObject(play1);

	Button* play2 = new Button("Tank", sf::Color{ 18, 113, 230 }, Vector2(0.0f, 0.0f), Vector2(16.0f, 16.0f));
	play2->SetAction([&hasToPlaySong]() {
		hasToPlaySong = true;
		SM.SetNextScene("Gameplay Tank");
		});
	play2->GetTransform()->scale = Vector2(1.75f, 1.75f);
	play2->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f, 250.0f);
	play2->GetBackground()->GetTransform()->position = play2->GetTransform()->position;
	SPAWN.SpawnObject(play2->GetBackground());
	SPAWN.SpawnObject(play2);

	Button* play3 = new Button("Splat", sf::Color{ 18, 113, 230 }, Vector2(0.0f, 0.0f), Vector2(16.0f, 16.0f));
	play3->SetAction([&hasToPlaySong]() {
		hasToPlaySong = true;
		SM.SetNextScene("Gameplay Swatter");
		});
	play3->GetTransform()->scale = Vector2(1.75f, 1.75f);
	play3->GetTransform()->position = Vector2((RM->WINDOW_WIDTH / 2.0f) + 250.0f, 250.0f);
	play3->GetBackground()->GetTransform()->position = play3->GetTransform()->position;
	SPAWN.SpawnObject(play3->GetBackground());
	SPAWN.SpawnObject(play3);
}

void SpawnSpriteSelect() {
	Button* selector = new Button("Sprites", sf::Color{ 18, 113, 230 }, Vector2(0.0f, 0.0f), Vector2(16.0f, 16.0f));
	selector->SetAction([]() {
		SM.SetNextScene("Sprite Selector");
		});
	selector->GetTransform()->scale = Vector2(1.75f, 1.75f);
	selector->GetTransform()->position = Vector2((RM->WINDOW_WIDTH / 2.0f), 390.0f);
	selector->GetBackground()->GetTransform()->position = selector->GetTransform()->position;
	SPAWN.SpawnObject(selector->GetBackground());
	SPAWN.SpawnObject(selector);
}

void SpawnRanking() {
	Button* ranking = new Button("Ranking", sf::Color{ 18, 113, 230 }, Vector2(0.0f, 0.0f), Vector2(16.0f, 16.0f));
	ranking->SetAction([]() {
		SM.SetNextScene("Ranking Screen");
		});
	ranking->GetTransform()->scale = Vector2(1.75f, 1.75f);
	ranking->GetTransform()->position = Vector2((RM->WINDOW_WIDTH / 2.0f), 530.0f);
	ranking->GetBackground()->GetTransform()->position = ranking->GetTransform()->position;
	SPAWN.SpawnObject(ranking->GetBackground());
	SPAWN.SpawnObject(ranking);
}

void SpawnExit() {
	Button* exit = new Button("Exit", sf::Color{ 18, 113, 230 }, Vector2(0.0f, 0.0f), Vector2(16.0f, 16.0f));
	exit->SetAction([]() {
		Input.SetPlaying(false);
		});
	exit->GetTransform()->scale = Vector2(1.75f, 1.75f);
	exit->GetTransform()->position = Vector2((RM->WINDOW_WIDTH / 2.0f), 670.0f);
	exit->GetBackground()->GetTransform()->position = exit->GetTransform()->position;
	SPAWN.SpawnObject(exit->GetBackground());
	SPAWN.SpawnObject(exit);
}

void SpawnMuteUnMute() {
	VolumeButton* volume = new VolumeButton(" ", sf::Color{18, 113, 230}, "resources/volume.png", Vector2(0.0f, 0.0f), Vector2(300.0f, 300.0f));
	if (!AUDIO->GetMuted()) {
		volume->SetBackground(volume->CreateVolume());
	}
	else {
		volume->SetBackground(volume->CreateNoVolume());
	}
	volume->GetTransform()->scale = Vector2(0.3f, 0.3f);
	volume->GetBackground()->GetTransform()->scale = Vector2(0.3f, 0.3f);
	volume->GetTransform()->position = Vector2(RM->WINDOW_WIDTH - (volume->GetBackground()->GetTransform()->size.x * volume->GetBackground()->GetTransform()->scale.x),
		RM->WINDOW_HEIGHT - (volume->GetBackground()->GetTransform()->size.y * volume->GetBackground()->GetTransform()->scale.y));
	volume->GetBackground()->GetTransform()->position = volume->GetTransform()->position;
	SPAWN.SpawnObject(volume->GetBackground());
	SPAWN.SpawnObject(volume);
}

void MainMenu::OnEnter() {
	SpawnTitle();
	SpawnPlayButtons(hasToPlaySong);
	SpawnSpriteSelect();
	SpawnRanking();
	SpawnExit();
	SpawnMuteUnMute();
	if (hasToPlaySong) {
		AUDIO->PlaySong("main_menu", 10);
		hasToPlaySong = false;
	}
}

void MainMenu::OnExit() {
	Scene::OnExit();
}

void MainMenu::Update() {
	Scene::Update();
}