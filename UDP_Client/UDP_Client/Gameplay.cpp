#include "Gameplay.h"
#include "ImageObject.h"
#include "Spawner.h"
#include "Managers/SceneManager.h"
#include <iostream>
#include "Managers/InputManager.h"
#include "Managers/PacketManager.h"
#include "Maps/TileMap.h"
#include "player/LocalPlayer.h"

#define GRID_WIDTH 1000.0f
#define GRID_HEIGHT 800.0f
#define GRID_OFFSET_X 500.0f
#define GRID_OFFSET_Y 50.0f

void Gameplay::OnEnter() {
    //Pata obtener ID del jugador y el total de jugadores, para luego enviar la info a los demas
    //PM->SendPlayerInfoToAll();

    gameFinished = false;

    Object* background = new ImageObject("resources/Gameplay/GameplayBackground.png", Vector2{ 0, 0 }, Vector2{ 1536, 1024 });
    background->GetTransform()->position = { RM->WINDOW_WIDTH / 2.0f, RM->WINDOW_HEIGHT / 2.0f };
    background->GetTransform()->scale = { 1.0f, 1.0f };
    background->GetRigidbody()->ClearColliders();
    SPAWN.SpawnObject(background);

    float width = GRID_WIDTH / MAX_COLS;
    float height = GRID_HEIGHT / MAX_ROWS;

    TileMap tileMap;
    tileMap.LoadFromFile("resources/Maps/Map.txt");

    Player* player = new LocalPlayer("resources/Tilesets/knight.png", Vector2(0.0f, 0.0f), Vector2(32.0f, 32.0f), 0, 4, 0.1f, true, 1.0f, 3);
	player->GetTransform()->scale = Vector2(3.0f, 3.0f);
    player->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f, RM->WINDOW_HEIGHT / 2.0f);
    SPAWN.SpawnObject(player);
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
    Scene::Update();

    if (gameFinished) return;

    // Procesa las desconexiones de los jugadores
    // Actualizando el turno si es necesario y finalizando el juego si quedan 1 o ningún jugador activo
    //while (PM->HasPendingDisconnect()) {
    //    short disconnectedID = PM->PopPendingDisconnect();
    //    std::cout << "Player " << disconnectedID << " left the game" << std::endl;

    //    if (currentPlayerIDTurn == disconnectedID) {
    //        ChangeTurn();
    //        currentMoveTime = 0.0f;
    //    }

    //    if (ShouldFinishGame()) {
    //        FinishGame();
    //        return;
    //    }
    //}

    //if (CheckWinCondition(row, col)) {
    //    std::cout << "Player " << playerID << " wins!" << std::endl;
    //}

    //if (ShouldFinishGame()) {
    //    FinishGame();
    //    return;
    //}
}

void Gameplay::Render() {
    Scene::Render();
}