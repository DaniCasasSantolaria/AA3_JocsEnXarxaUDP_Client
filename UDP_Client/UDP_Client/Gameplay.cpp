#include "Gameplay.h"
#include "ImageObject.h"
#include "Spawner.h"
#include "Managers/SceneManager.h"
#include <iostream>
#include "Managers/InputManager.h"
#include "Managers/PacketManager.h"
#include "Maps/TileMap.h"
#include "player/LocalPlayer.h"
#include "Elements/Bullet.h"

#define GRID_WIDTH 1000.0f
#define GRID_HEIGHT 800.0f

void Gameplay::OnEnter() {
    gameFinished = false;

    Object* background = new ImageObject("resources/Gameplay/GameplayBackground.png", Vector2{ 0, 0 }, Vector2{ 1536, 1024 });
    background->GetTransform()->position = { RM->WINDOW_WIDTH / 2.0f, RM->WINDOW_HEIGHT / 2.0f };
    background->GetTransform()->scale = { 1.0f, 1.0f };
    background->GetRigidbody()->ClearColliders();
    SPAWN.SpawnObject(background);

    TileMap tileMap;
    tileMap.LoadFromFile("resources/Maps/Map.txt");

    localPlayer = new LocalPlayer("resources/Tilesets/knight.png", Vector2(0.0f, 0.0f), Vector2(32.0f, 32.0f), 0, 4, 0.1f, true, 1.0f, 3);
    localPlayer->GetTransform()->scale = Vector2(3.0f, 3.0f);
    localPlayer->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f - 200.0f, RM->WINDOW_HEIGHT - 100.0f);
    SPAWN.SpawnObject(localPlayer);

	onlinePlayer = new OnlinePlayer("resources/Tilesets/Enemyknight.png", Vector2(0.0f, 0.0f), Vector2(32.0f, 32.0f), 0, 4, 0.1f, true, 1.0f, 3);
	onlinePlayer->GetTransform()->scale = Vector2(3.0f, 3.0f);
    onlinePlayer->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f - 200.0f, RM->WINDOW_HEIGHT - 100.0f);
    SPAWN.SpawnObject(onlinePlayer);
}

void Gameplay::OnExit() {
    Scene::OnExit();
}

bool Gameplay::CheckWinCondition(short row, short col) {
    //short player = grid[row][col]->playerID;
    //if (player == -1)
    //    return false;

    //const short directions[4][2] = {
    //    {0, 1},   // Horizontal
    //    {1, 0},   // Vertical
    //    {1, 1},   // Diagonal
    //    {1, -1}   // Diagonal
    //};

    //for (unsigned short i = 0; i < 4; i++) {
    //    short dRow = directions[i][0];
    //    short dCol = directions[i][1];

    //    short total = 1 + CountDirection(row, col, dRow, dCol, player) + CountDirection(row, col, -dRow, -dCol, player);

    //    if (total >= 3) {
    //        PM->RecordWinner(player);
    //        return true;
    //    }
    //}

    //return false;
}

// Verifica si el juego debe finalizar si queda 1 o ningún jugador activo o si se han colocado todas las piezas
bool Gameplay::ShouldFinishGame() const {
    /*short activePlayers = maxPlayers - PM->GetFinishedCount() - static_cast<short>(PM->disconnectedPlayers.size());
    return activePlayers <= 1 || piecesPlaced >= MAX_ROWS * MAX_COLS;*/
}

// Finaliza el juego desconectando a los jugadores
void Gameplay::FinishGame()
{
   /* if (gameFinished) return;
    gameFinished = true;

    for (unsigned short i = 0; i < maxPlayers; i++) {
        if (!PM->HasFinished(i) && !PM->IsDisconnected(i)) {
            PM->RecordWinner(i);
        }
    }

    PM->DisconnectPeers();
    PM->DisconnectFromServer();

    if (PM->ConnectToServer()) {
        PM->SendGameResult();
        SM.SetNextScene("Lobby");
    }
    else {
        std::cerr << "Failed to reconnect to server!" << std::endl;
    }*/
}

void Gameplay::Update() {
    if (onlinePlayer != nullptr) {
        while (PM->HasPendingOnlineMovement()) {
            PacketManager::OnlineMovement movement = PM->PopPendingOnlineMovement();

            onlinePlayer->AddServerMovement(
                movement.movementId,
                movement.position
            );
        }
    }

    while (PM->HasPendingLocalValidation() && localPlayer != nullptr) {
        PacketManager::LocalValidation validation = PM->PopPendingLocalValidation();

        localPlayer->ApplyServerValidation(
            validation.movementId,
            validation.position
        );
    }


    while (PM->HasPendingShoot()) {
        PacketManager::ShootData shootData = PM->PopPendingShoot();

        Bullet* bullet = new Bullet(
            "resources/bullet.png",
            Vector2(shootData.directionX, shootData.directionY),
            shootData.shooterNetworkId,
            600.0f
        );
        bullet->GetTransform()->position = Vector2(shootData.spawnX, shootData.spawnY);
        SPAWN.SpawnObject(bullet);
    }

    Scene::Update();

    if (localPlayer != nullptr) {
        localPlayer->TrySendMovement();
    }

    if (gameFinished) return;
}

void Gameplay::Render() {
    Scene::Render();
}