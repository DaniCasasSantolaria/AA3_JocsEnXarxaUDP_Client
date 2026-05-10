#include "Gameplay.h"
#include "ImageObject.h"
#include "Spawner.h"
#include "Managers/SceneManager.h"
#include <iostream>
#include "Managers/InputManager.h"
#include "Managers/PacketManager.h"
#include "Maps/TileMap.h"

#define GRID_WIDTH 1000.0f
#define GRID_HEIGHT 800.0f
#define GRID_OFFSET_X 500.0f
#define GRID_OFFSET_Y 50.0f

void Gameplay::OnEnter() {
    //Pata obtener ID del jugador y el total de jugadores, para luego enviar la info a los demas
    myPlayerID = PM->GetMyIndex();
    maxPlayers = PM->GetTotalPlayers();
    PM->SendPlayerInfoToAll();

    //Turno inicial siempre al jugador 0 que es el que crea la sala
    currentPlayerIDTurn = 0;
    piecesPlaced = 0;
    gameFinished = false;

    //Se guarda la info de los jugadores en arrays locales para luego mostrarla en pantalla
    usernamesByIndex[myPlayerID] = PM->myUsername;
    scoreByIndex[myPlayerID] = PM->playerInfo.score;

    Object* background = new ImageObject("resources/Gameplay/GameplayBackground.png", Vector2{ 0, 0 }, Vector2{ 1536, 1024 });
    background->GetTransform()->position = { RM->WINDOW_WIDTH / 2.0f, RM->WINDOW_HEIGHT / 2.0f };
    background->GetTransform()->scale = { 1.0f, 1.0f };
    SPAWN.SpawnObject(background);

    float width = GRID_WIDTH / MAX_COLS;
    float height = GRID_HEIGHT / MAX_ROWS;

    TileMap tileMap;
    tileMap.LoadFromFile("resources/Maps/1/Map1.txt");
}

void Gameplay::OnExit() {
    Scene::OnExit();
}

void Gameplay::OnGridBoxPressed(short row, short col) {
    if (!IsMyTurn()) return;

    std::cout << "Grid box pressed: Row " << row << ", Col " << col << std::endl;
    if (!grid[row][col]->isEmpty) return;

    //Se cambia el estado de la casilla localmente
    ChangeGridBox(row, col, myPlayerID);
    ChangeTurn();
    piecesPlaced++;
    currentMoveTime = 0;

    //Se envia a los demas jugadores para que ellos actualicen su estado de la casilla
    PM->SendTurnAction(row, col, myPlayerID);

    //Mira si ha ganado el jugador que ha hecho la jugada
    if (CheckWinCondition(row, col)) {
        std::cout << "Player " << myPlayerID << " Win the Game!" << std::endl;
        // Se envia la notificacion a los demas para que actualicen su estado de la partida
        PM->SendWinNotificationToAll(myPlayerID);
    }

    if (ShouldFinishGame()) {
        FinishGame();
    }
}

//Fet amb ChatGPT, per calcular la ll�gica de les combinacions del joc
bool Gameplay::IsInside(short row, short col) const {
    return row >= 0 && row < MAX_ROWS && col >= 0 && col < MAX_COLS;
}

short Gameplay::CountDirection(short row, short col, short dRow, short dCol, short player) const {
    short count = 0;

    row += dRow;
    col += dCol;

    while (IsInside(row, col) && grid[row][col]->playerID == player) {
        count++;
        row += dRow;
        col += dCol;
    }

    return count;
}

bool Gameplay::CheckWinCondition(short row, short col) {
    short player = grid[row][col]->playerID;
    if (player == -1)
        return false;

    const short directions[4][2] = {
        {0, 1},   // Horizontal
        {1, 0},   // Vertical
        {1, 1},   // Diagonal
        {1, -1}   // Diagonal
    };

    for (unsigned short i = 0; i < 4; i++) {
        short dRow = directions[i][0];
        short dCol = directions[i][1];

        short total = 1 + CountDirection(row, col, dRow, dCol, player) + CountDirection(row, col, -dRow, -dCol, player);

        if (total >= 3) {
            PM->RecordWinner(player);
            return true;
        }
    }

    return false;
}

//Actualiza la casilla del tablero
void Gameplay::ChangeGridBox(short row, short col, short playerID) {
    grid[row][col]->isEmpty = false;
    grid[row][col]->playerID = playerID;
    grid[row][col]->ApplyImage();
}

// Cambia el turno al siguiente jugador activo
void Gameplay::ChangeTurn() {
    unsigned short attempts = 0;
    do {
        currentPlayerIDTurn = (currentPlayerIDTurn + 1) % maxPlayers;
        attempts++;
    } while (attempts < maxPlayers &&
        (PM->HasFinished(currentPlayerIDTurn) || PM->IsDisconnected(currentPlayerIDTurn) ||
            (!PM->IsPeerConnected(currentPlayerIDTurn) && currentPlayerIDTurn != myPlayerID)));
}

// Verifica si el juego debe finalizar si queda 1 o ningún jugador activo o si se han colocado todas las piezas
bool Gameplay::ShouldFinishGame() const {
    short activePlayers = maxPlayers - PM->GetFinishedCount() - static_cast<short>(PM->disconnectedPlayers.size());
    return activePlayers <= 1 || piecesPlaced >= MAX_ROWS * MAX_COLS;
}

// Finaliza el juego desconectando a los jugadores
void Gameplay::FinishGame()
{
    if (gameFinished) return;
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
    }
}

void Gameplay::Update() {
    Scene::Update();

    if (gameFinished) return;

    // Controla el tiempo de movimiento, si se acaba el tiempo se salta el turno
    currentMoveTime += TIME.GetDeltaTime();
    if (currentMoveTime >= maxTimeToMove) {
        std::cout << "Player " << currentPlayerIDTurn << " ran out of time! Skipping turn." << std::endl;
        if (IsMyTurn()) {
            PM->SendTurnAction(-2, -2, myPlayerID);
        }
        ChangeTurn();
        currentMoveTime = 0.0f;
    }

    // Procesa la información de los jugadores que llega del servidor, actualizando los nombres y puntuaciones en pantalla
    while (PM->HasPendingPlayerInfo()) {
        std::pair<short, PlayerInfo> action = PM->PopPendingPlayerInfo();

        usernamesByIndex[action.first] = action.second.username;
        scoreByIndex[action.first] = action.second.score;

        if (playerInfoText[action.first]) {
            playerInfoText[action.first]->SetText(action.second.username);
        }
        if (scoreInfoText[action.first]) {
            scoreInfoText[action.first]->SetText(std::to_string(action.second.score));
        }
    }

    // Procesa las desconexiones de los jugadores
    // Actualizando el turno si es necesario y finalizando el juego si quedan 1 o ningún jugador activo
    while (PM->HasPendingDisconnect()) {
        short disconnectedID = PM->PopPendingDisconnect();
        std::cout << "Player " << disconnectedID << " left the game" << std::endl;

        if (currentPlayerIDTurn == disconnectedID) {
            ChangeTurn();
            currentMoveTime = 0.0f;
        }

        if (ShouldFinishGame()) {
            FinishGame();
            return;
        }
    }

    // Procesa las acciones de los jugadores que llegan del servidor
    // Actualizando el estado del tablero y verificando condiciones de victoria o finalización del juego
    while (PM->HasPendingAction()) {
        std::pair<short, sf::Packet> action = PM->PopPendingAction();
        short row, col, playerID;
        action.second >> row >> col >> playerID;
        ChangeTurn();
        if (row < 0 || col < 0) {
            std::cout << "Turn skipped for player " << playerID << std::endl;
            continue;
        }
        ChangeGridBox(row, col, playerID);
        piecesPlaced++;
        currentMoveTime = 0.0f;

        if (CheckWinCondition(row, col)) {
            std::cout << "Player " << playerID << " wins!" << std::endl;
        }

        if (ShouldFinishGame()) {
            FinishGame();
            return;
        }
    }
}

void Gameplay::Render() {
    Scene::Render();
}