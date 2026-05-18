#include "Game.h"
#include "Managers/RenderManager.h"
#include "Managers/SceneManager.h"
#include "Gameplay.h"
#include "MainMenu.h"
#include "SplashScreen.h"
#include "scenes/Login.h"
#include "scenes/Lobby.h"
#include "Managers/AudioManager.h"
#include "scenes/RankingScreen.h"
#include <cassert>

#include "scenes/GameplaySpaceship.h"	
#include "Managers/PacketManager.h"

void Game::Init() {
	RM->Init();
	PM->ConnectToServer();

	PM->RequestMap();

	//IMAGES
	RM->LoadTexture("resources/Tres.png");

	//Login
	RM->LoadTexture("resources/Login/login-button.png");
	RM->LoadTexture("resources/Login/Password-button.png");
	RM->LoadTexture("resources/Login/register-button.png");
	RM->LoadTexture("resources/Login/user-button.png");
	RM->LoadTexture("resources/Login/hide.png");
	RM->LoadTexture("resources/Login/show.png");

	//Lobby
	RM->LoadTexture("resources/Lobby/lobby-button.png");
	RM->LoadTexture("resources/Lobby/join-button.png");
	RM->LoadTexture("resources/Lobby/create-button.png");

	//Gameplay
	RM->LoadTexture("resources/Gameplay/GameplayBackground.png");
	RM->LoadTexture("resources/Gameplay/GridBackground.png");
	RM->LoadTexture("resources/Gameplay/Grid.png");

	RM->LoadTexture("resources/Gameplay/GridCircle.png");
	RM->LoadTexture("resources/Gameplay/GridTriangle.png");
	RM->LoadTexture("resources/Gameplay/GridSquare.png");
	RM->LoadTexture("resources/Gameplay/GridCross.png");

	RM->LoadTexture("resources/Gameplay/CirclePlayerRectangle.png");
	RM->LoadTexture("resources/Gameplay/TrianglePlayerRectangle.png");
	RM->LoadTexture("resources/Gameplay/SquarePlayerRectangle.png");
	RM->LoadTexture("resources/Gameplay/CrossPlayerRectangle.png");

	RM->LoadTexture("resources/Tilesets/world_tileset.png");
	RM->LoadTexture("resources/Tilesets/platforms.png");

	RM->LoadTexture("resources/Tilesets/knight.png");
	
	//Backgrounds
	RM->LoadTexture("resources/Login/Fondo.png");
	
	//VFX
	//RM->LoadTexture("resources/bullets/bullet_1.png");

	//AUDIO
	
	//MUSIC
	//AUDIO->LoadSong("main_menu");

	//SFX
	AUDIO->LoadClip("hover");

	//INIT SCENES
	SM.AddScene("Login", new Login());
	SM.AddScene("Lobby", new Lobby());
	SM.AddScene("Ranking Screen", new RankingScreen());
	SM.AddScene("Gameplay", new Gameplay());
	/*//assert(SM.AddScene("Splash Screen", new SplashScreen()));
	//assert(SM.AddScene("Ranking Screen", new RankingScreen()));*/

	//INIT FIRST SCENE
	SM.InitFirstScene("Login");
}

void Game::Update() {
	PM->Update();
	SM.UpdateCurrentScene();
}

void Game::Render() {
	RM->ClearScreen();

	SM.GetCurrentScene()->Render();

	RM->RenderScreen();
}

void Game::Release() {
	SM.GetCurrentScene()->OnExit();

	RM->Release();
}