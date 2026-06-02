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
    localPlayer->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f, RM->WINDOW_HEIGHT / 2.0f);
    SPAWN.SpawnObject(localPlayer);

	onlinePlayer = new OnlinePlayer("resources/Tilesets/Enemyknight.png", Vector2(0.0f, 0.0f), Vector2(32.0f, 32.0f), 0, 4, 0.1f, true, 1.0f, 3);
	onlinePlayer->GetTransform()->scale = Vector2(3.0f, 3.0f);
    onlinePlayer->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f, RM->WINDOW_HEIGHT / 2.0f);
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

void Gameplay::PrintHealthDebug(const char* reason) {
    if (localPlayer == nullptr || onlinePlayer == nullptr) {
        return;
    }

    std::cout << reason << std::endl;

    std::cout << "Current HP: " << localPlayer->GetCurrentHealthPoints()
              << " Current Lives: " << localPlayer->GetCurrentLives() << std::endl
              << "Enemy HP: " << onlinePlayer->GetCurrentHealthPoints()
              << " Enemy Lives: " << onlinePlayer->GetCurrentLives() << std::endl;
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

    if (Input.GetEvent(sf::Keyboard::Key::H, KeyState::DOWN)) {
        localPlayer->RecieveDamage(1);

        std::cout << "Current HP: " << localPlayer->GetCurrentHealthPoints() << " Current Lives: " << localPlayer->GetCurrentLives() << std::endl
            << "Enemy HP: " << onlinePlayer->GetCurrentHealthPoints() << " Enemy Lives: " << onlinePlayer->GetCurrentLives() << std::endl;
    }

    if (!gameFinished && PM->HasMatchFinished()) {
        gameFinished = true;
        resultTimer = 0.0f;

        std::string text;

        if (PM->GetLastMatchResult() == MATCH_RESULT_WIN) {
            text = "YOU WIN";
        }
        else {
            text = "YOU LOSE";
        }

        resultText = new TextObject(text, sf::Color::White);
        resultText->GetTransform()->position = Vector2(
            RM->WINDOW_WIDTH / 2.0f,
            RM->WINDOW_HEIGHT / 2.0f
        );
        resultText->GetTransform()->scale = Vector2(3.0f, 3.0f);

        SPAWN.SpawnObject(resultText);

        PM->ClearMatchFinished();
    }

    if (gameFinished) {
        Scene::Update();

        resultTimer += TIME.GetDeltaTime();

        if (resultTimer >= RESULT_TIME) {
            SM.SetNextScene("Lobby");
        }

        return;
    }

    if (onlinePlayer != nullptr) {
        while (PM->HasPendingOnlineMovement()) {
            PacketManager::OnlineMovement movement = PM->PopPendingOnlineMovement();

            onlinePlayer->AddServerMovement(
                movement.movementId,
                movement.position
            );
        }

        while (PM->HasPendingEnemyHealthUpdate()) {
            PacketManager::EnemyHealthUpdate healthUpdate = PM->PopPendingEnemyHealthUpdate();

            onlinePlayer->SetCurrentLives(healthUpdate.lives);
            onlinePlayer->SetCurrentHealthPoints(healthUpdate.health);

            PrintHealthDebug("YOU Hit the Enemy");
        }
    }

    while (PM->HasPendingLocalValidation() && localPlayer != nullptr) {
        PacketManager::LocalValidation validation = PM->PopPendingLocalValidation();

        localPlayer->ApplyServerValidation(
            validation.movementId,
            validation.position
        );
    }


    /*while (PM->HasPendingHitConfirmation()) {
        PacketManager::HitConfirmedData hitData = PM->PopPendingHitConfirmation();
        if (hitData.targetPlayerId == PM->GetMyIndex() && localPlayer != nullptr) {
            localPlayer->RecieveDamage(1);
        }
    }*/

    while (PM->HasPendingShoot()) {
        PacketManager::ShootData shootData = PM->PopPendingShoot();

        if (onlinePlayer != nullptr) {
            Vector2 bulletDirection = onlinePlayer->IsLookingRight() ? Vector2(1.0f, 0.0f) : Vector2(-1.0f, 0.0f);
            float spawnOffsetX = (onlinePlayer->GetTransform()->size.x * (onlinePlayer->GetTransform()->scale.x > 0.0f ? onlinePlayer->GetTransform()->scale.x : -onlinePlayer->GetTransform()->scale.x)) / 2.0f;
            Vector2 spawnPosition = Vector2(
                onlinePlayer->GetTransform()->position.x + bulletDirection.x * spawnOffsetX,
                onlinePlayer->GetTransform()->position.y
            );

            Bullet* bullet = new Bullet("resources/bullet.png", bulletDirection, shootData.shooterNetworkId, 600.0f);
            bullet->GetTransform()->position = spawnPosition;
            SPAWN.SpawnObject(bullet);
        }
    }

    while (PM->HasShootConfirmed()) {
        PM->ConsumeShootConfirmed();

        if (localPlayer != nullptr) {
            Vector2 bulletDirection = localPlayer->IsLookingRight() ? Vector2(1.0f, 0.0f) : Vector2(-1.0f, 0.0f);
            float spawnOffsetX = (localPlayer->GetTransform()->size.x * (localPlayer->GetTransform()->scale.x > 0.0f ? localPlayer->GetTransform()->scale.x : -localPlayer->GetTransform()->scale.x)) / 2.0f;
            Vector2 spawnPosition = Vector2(
                localPlayer->GetTransform()->position.x + bulletDirection.x * spawnOffsetX,
                localPlayer->GetTransform()->position.y
            );

            Bullet* bullet = new Bullet("resources/bullet.png", bulletDirection, PM->GetMyIndex(), 600.0f);
            bullet->GetTransform()->position = spawnPosition;
            SPAWN.SpawnObject(bullet);
        }
    }

    Scene::Update();

    if (localPlayer != nullptr && localPlayer->ConsumePendingHealthDebugPrint()) {
        PrintHealthDebug("YOU recive damage");
    }

    if (localPlayer != nullptr) {
        localPlayer->TrySendMovement();
    }

    if (gameFinished) return;
}

void Gameplay::Render() {
    Scene::Render();
}