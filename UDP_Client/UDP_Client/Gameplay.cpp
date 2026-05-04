#include "Gameplay.h"
#include "ImageObject.h"
#include "Spawner.h"
#include "Managers/SceneManager.h"
#include <iostream>
#include "Managers/InputManager.h"
#include "Managers/PacketManager.h"

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


    //Tablero
    for (int row = 0; row < MAX_ROWS; row++) {
        for (int col = 0; col < MAX_COLS; col++) {

            grid[row][col] = new GridBox("resources/Gameplay/Grid.png", Vector2{ 0, 0 }, Vector2{ width, height });
            grid[row][col]->SetAction([this, row, col]() {
                OnGridBoxPressed(row, col);
                });
            grid[row][col]->GetTransform()->position = { GRID_OFFSET_X + col * width + width / 2.0f, GRID_OFFSET_Y + row * height + height / 2.0f };
            grid[row][col]->GetTransform()->scale = { 1.0f, 1.0f };
            grid[row][col]->isEmpty = true;
            grid[row][col]->playerID = -1;

            SPAWN.SpawnObject(grid[row][col]);
        }
    }

    Object* gridBackground = new ImageObject("resources/Gameplay/GridBackground.png", Vector2{ 0, 0 }, Vector2{ GRID_WIDTH + 35, GRID_HEIGHT + 35 });
    gridBackground->GetTransform()->position = { GRID_OFFSET_X + GRID_WIDTH / 2.0f, GRID_OFFSET_Y + GRID_HEIGHT / 2.0f };
    gridBackground->GetTransform()->scale = { 1.0f, 1.0f };
    SPAWN.SpawnObject(gridBackground);


	Object* player1Info = new ImageObject("resources/Gameplay/CirclePlayerRectangle.png", Vector2{ 0, 0 }, Vector2{ 300, 100 });
    player1Info->GetTransform()->position = { 200, 100 };
    player1Info->GetTransform()->scale = { 1.0f, 1.0f };
    SPAWN.SpawnObject(player1Info);

    Object* player2Info = new ImageObject("resources/Gameplay/TrianglePlayerRectangle.png", Vector2{ 0, 0 }, Vector2{ 300, 100 });
    player2Info->GetTransform()->position = { 200, 250 };
    player2Info->GetTransform()->scale = { 1.0f, 1.0f };
    SPAWN.SpawnObject(player2Info);

    Object* player3Info = new ImageObject("resources/Gameplay/SquarePlayerRectangle.png", Vector2{ 0, 0 }, Vector2{ 300, 100 });
    player3Info->GetTransform()->position = { 200, 400 };
    player3Info->GetTransform()->scale = { 1.0f, 1.0f };
    SPAWN.SpawnObject(player3Info);

    Object* player4Info = new ImageObject("resources/Gameplay/CrossPlayerRectangle.png", Vector2{ 0, 0 }, Vector2{ 300, 100 });
    player4Info->GetTransform()->position = { 200, 550 };
    player4Info->GetTransform()->scale = { 1.0f, 1.0f };
    SPAWN.SpawnObject(player4Info);

    int playersNameScale = 1.9f;
	int playersScoreScale = 1.4f;

    playerInfoText[0] = new TextObject(usernamesByIndex[0], sf::Color{ 255,255,255 });
    playerInfoText[0]->GetTransform()->scale = Vector2(playersNameScale, playersNameScale);
    playerInfoText[0]->GetTransform()->position = Vector2(240.0f, 80.0f);
    SPAWN.SpawnObject(playerInfoText[0]);

    playerInfoText[1] = new TextObject(usernamesByIndex[1], sf::Color{ 255,255,255 });
    playerInfoText[1]->GetTransform()->scale = Vector2(playersNameScale, playersNameScale);
    playerInfoText[1]->GetTransform()->position = Vector2(240.0f, 230.0f);
    SPAWN.SpawnObject(playerInfoText[1]);

    playerInfoText[2] = new TextObject(usernamesByIndex[2], sf::Color{ 255,255,255 });
    playerInfoText[2]->GetTransform()->scale = Vector2(playersNameScale, playersNameScale);
    playerInfoText[2]->GetTransform()->position = Vector2(240.0f, 380.0f);
    SPAWN.SpawnObject(playerInfoText[2]);

    playerInfoText[3] = new TextObject(usernamesByIndex[3], sf::Color{ 255,255,255 });
    playerInfoText[3]->GetTransform()->scale = Vector2(playersNameScale, playersNameScale);
    playerInfoText[3]->GetTransform()->position = Vector2(240.0f, 530.0f);
    SPAWN.SpawnObject(playerInfoText[3]);

    scoreInfoText[0] = new TextObject(std::to_string(scoreByIndex[0]), sf::Color{ 255,255,255 });
    scoreInfoText[0]->GetTransform()->scale = Vector2(playersScoreScale, playersScoreScale);
    scoreInfoText[0]->GetTransform()->position = Vector2(240.0f, 115.0f);
    SPAWN.SpawnObject(scoreInfoText[0]);

    scoreInfoText[1] = new TextObject(std::to_string(scoreByIndex[1]), sf::Color{ 255,255,255 });
    scoreInfoText[1]->GetTransform()->scale = Vector2(playersScoreScale, playersScoreScale);
    scoreInfoText[1]->GetTransform()->position = Vector2(240.0f, 265.0f);
    SPAWN.SpawnObject(scoreInfoText[1]);

    scoreInfoText[2] = new TextObject(std::to_string(scoreByIndex[2]), sf::Color{ 255,255,255 });
    scoreInfoText[2]->GetTransform()->scale = Vector2(playersScoreScale, playersScoreScale);
    scoreInfoText[2]->GetTransform()->position = Vector2(240.0f, 415.0f);
    SPAWN.SpawnObject(scoreInfoText[2]);

    scoreInfoText[3] = new TextObject(std::to_string(scoreByIndex[3]), sf::Color{ 255,255,255 });
    scoreInfoText[3]->GetTransform()->scale = Vector2(playersScoreScale, playersScoreScale);
    scoreInfoText[3]->GetTransform()->position = Vector2(240.0f, 565.0f);
    SPAWN.SpawnObject(scoreInfoText[3]);
}

void Gameplay::OnExit() {
	Scene::OnExit();
}

void Gameplay::OnGridBoxPressed(int row, int col) {
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
bool Gameplay::IsInside(int row, int col) const {
    return row >= 0 && row < MAX_ROWS && col >= 0 && col < MAX_COLS;
}

int Gameplay::CountDirection(int row, int col, int dRow, int dCol, int player) const {
    int count = 0;

    row += dRow;
    col += dCol;

    while (IsInside(row, col) && grid[row][col]->playerID == player) {
        count++;
        row += dRow;
        col += dCol;
    }

    return count;
}

bool Gameplay::CheckWinCondition(int row, int col) {
    int player = grid[row][col]->playerID;
    if (player == -1)
        return false;

    const int directions[4][2] = {
        {0, 1},   // Horizontal
        {1, 0},   // Vertical
        {1, 1},   // Diagonal
        {1, -1}   // Diagonal
    };

    for (int i = 0; i < 4; i++) {
        int dRow = directions[i][0];
        int dCol = directions[i][1];

        int total = 1 + CountDirection(row, col, dRow, dCol, player) + CountDirection(row, col, -dRow, -dCol, player);

        if (total >= 3) {
            PM->RecordWinner(player);
            return true;
        }
    }

    return false;
}

//Actualiza la casilla del tablero
void Gameplay::ChangeGridBox(int row, int col, int playerID) {
    grid[row][col]->isEmpty = false;
    grid[row][col]->playerID = playerID;
    grid[row][col]->ApplyImage();
}

// Cambia el turno al siguiente jugador activo
void Gameplay::ChangeTurn() {
    int attempts = 0;
    do {
        currentPlayerIDTurn = (currentPlayerIDTurn + 1) % maxPlayers;
        attempts++;
    } while (attempts < maxPlayers &&
             (PM->HasFinished(currentPlayerIDTurn) || PM->IsDisconnected(currentPlayerIDTurn) ||
              (!PM->IsPeerConnected(currentPlayerIDTurn) && currentPlayerIDTurn != myPlayerID)));
}

// Verifica si el juego debe finalizar si queda 1 o ningún jugador activo o si se han colocado todas las piezas
bool Gameplay::ShouldFinishGame() const {
    int activePlayers = maxPlayers - PM->GetFinishedCount() - (int)PM->disconnectedPlayers.size();
    return activePlayers <= 1 || piecesPlaced >= MAX_ROWS * MAX_COLS;
}

// Finaliza el juego desconectando a los jugadores
void Gameplay::FinishGame()
{
    if (gameFinished) return;
    gameFinished = true;

    for (int i = 0; i < maxPlayers; i++) {
        if (!PM->HasFinished(i) && !PM->IsDisconnected(i)) {
            PM->RecordWinner(i);
        }
    }

    PM->DisconnectPeers();
    PM->DisconnectFromServer();

    if (PM->ConnectToServer()) {
        PM->SendGameResult();
        SM.SetNextScene("Lobby");
    } else {
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
        std::pair<int, PlayerInfo> action = PM->PopPendingPlayerInfo();

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
        int disconnectedID = PM->PopPendingDisconnect();
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
		std::pair<int, sf::Packet> action = PM->PopPendingAction();
        int row, col, playerID;
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