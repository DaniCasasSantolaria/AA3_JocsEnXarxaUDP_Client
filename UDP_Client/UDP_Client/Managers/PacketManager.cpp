#include "PacketManager.h"
#include "SceneManager.h"

#include <iostream>
#include "../Maps/TileMap.h"

sf::Packet& operator <<(sf::Packet& packet, packetType type) {
	return packet << static_cast<short>(type);
}

sf::Packet& operator <<(sf::Packet& packet, authResult result) {
	return packet << static_cast<short>(result);
}

sf::Packet& operator <<(sf::Packet& packet, matchMode mode) {
	return packet << static_cast<short>(mode);
}

sf::Packet& operator <<(sf::Packet& packet, matchmakeStatus status) {
	return packet << static_cast<short>(status);
}

sf::Packet& operator >>(sf::Packet& packet, packetType& type) {
	short  temp;
	packet >> temp;
	type = static_cast<packetType>(temp);
	return packet;
}

sf::Packet& operator >>(sf::Packet& packet, authResult& result) {
	short  temp;
	packet >> temp;
	result = static_cast<authResult>(temp);
	return packet;
}

sf::Packet& operator >>(sf::Packet& packet, matchMode& mode) {
	short  temp;
	packet >> temp;
	mode = static_cast<matchMode>(temp);
	return packet;
}

sf::Packet& operator >>(sf::Packet& packet, matchmakeStatus& status) {
	short  temp;
	packet >> temp;
	status = static_cast<matchmakeStatus>(temp);
	return packet;
}

// Establece conexión TCP con el servidor central, desconecta primero si ya hay una conexión activa
bool PacketManager::ConnectToServer() {
	if (serverConnected) {
		DisconnectFromServer();
	}

	if (socket.connect(SERVER_IP, LISTENER_PORT) != sf::Socket::Status::Done) {
		std::cerr << "Failed to connect to server" << std::endl;
		return false;
	}
	socket.setBlocking(false);
	serverConnected = true;
	std::cout << "Connected to server" << std::endl;
	return true;
}

// Cierra la conexión con el servidor, reinicia el socket y actualiza el estado de conexión
void PacketManager::DisconnectFromServer() {
	if (serverConnected) {
		socket.disconnect();
		socket = sf::TcpSocket();
		serverConnected = false;
		std::cout << "Disconnected from server" << std::endl;
	}
}

void PacketManager::Update() {
	// Recibir paquetes del servidor
	if (serverConnected) {
		sf::Packet packet;
		if (socket.receive(packet) == sf::Socket::Status::Done) {
			packetType type;
			packet >> type;
			switch (type) {
			case HANDSHAKE:
				HandShake(packet);
				break;
			case LOGIN:
				Login(packet);
				break;
			case REGISTER:
				Register(packet);
				break;
			case RANKING:
				GetRanking(packet);
				break;
			case MATCHMAKE:
				Matchmake(packet);
				break;
			case MAP_DATA:
				MapData(packet);
				break;
			default:
				break;
			}
			packet.clear();
		}
	}
}

// Paquete de handshake del servidor
void PacketManager::HandShake(sf::Packet& data) {
	std::string reciveMessage;
	data >> reciveMessage;
	std::cout << "Message from server: " << reciveMessage << std::endl;
}

// Respuesta de login del servidor, extrae credenciales, resultado de autenticación y puntuación
void PacketManager::Login(sf::Packet& data) {
	std::string user;
	std::string pass;
	authResult result;
	int score = 0;

	data >> user;
	data >> pass;
	data >> result;
	data >> score;

	playerInfo.username = user;
	playerInfo.score = score;


	switch (result) {
	case LOGIN_OK:
		myUsername = user;
		std::cout << "Login successful" << std::endl;
		SM.SetNextScene("Lobby");
		break;
	case USER_NOT_FOUND:
		std::cout << "User not found" << std::endl;
		break;
	case WRONG_PASSWORD:
		std::cout << "Wrong password" << std::endl;
		break;
	default:
		std::cout << "Unknown authentication response" << std::endl;
		break;
	}
}

// Respuesta de registro del servidor, extrae credenciales y resultado de autenticación
void PacketManager::Register(sf::Packet& data) {
	std::string user;
	std::string pass;
	authResult result;

	data >> user;
	data >> pass;
	data >> result;

	switch (result) {
	case REGISTER_OK:
		std::cout << "Registration successful" << std::endl;
		break;
	case USER_ALREADY_EXISTS:
		std::cout << "User already exists" << std::endl;
		break;
	default:
		std::cout << "Unknown authentication response" << std::endl;
		break;
	}
}

void PacketManager::Matchmake(sf::Packet& data) {
	matchMode mode;
	matchmakeStatus status;
	data >> mode;
	data >> status;
	switch (mode)
	{
	case COMPETITIVE:
		if (status == QUEUE_WAITING) {
			std::cout << "Added to competitive matchmaking queue. Waiting for match..." << std::endl;
		}
		else {
			std::cout << "Competitive match found. Opening UDP socket..." << std::endl;
		}
		break;
	case NON_COMPETITIVE:
		if(status == QUEUE_WAITING) {
			std::cout << "Added to non-competitive matchmaking queue. Waiting for match..." << std::endl;
		}
		else {
			std::cout << "Non-competitive match found. Opening UDP socket..." << std::endl;
		}
		break;
	default:
		std::cout << "Unknown matchmaking mode" << std::endl;
		break;
	}
}

void PacketManager::MapData(sf::Packet& data) {
	unsigned short mapCount = 0;
	data >> mapCount;

	for (unsigned short i = 0; i < mapCount; i++) {
		std::string relativePath;
		std::string mapContent;

		data >> relativePath >> mapContent;

		std::string localPath = "resources/Maps/" + relativePath;
		std::cout << localPath << std::endl;

		TileMap tileMap;
		tileMap.UpdateLocalMap(localPath, mapContent);
	}
}

// Envía solicitud de login al servidor con usuario y contraseña
void PacketManager::SendLoginRequest(const std::string& username, const std::string& password) {
	sf::Packet packet;
	packetType type = LOGIN;
	packet << type << username << password;
	if (socket.send(packet) != sf::Socket::Status::Done) {
		std::cerr << "Failed to send login data to server" << std::endl;
	}
	else {
		std::cout << "Login data sent to server: " << std::endl << username << std::endl << password << std::endl;
	}
}

// Envía solicitud de registro al servidor con usuario y contraseña
void PacketManager::SendRegisterRequest(const std::string& username, const std::string& password) {
	sf::Packet packet;
	packetType type = REGISTER;
	packet << type << username << password;
	if (socket.send(packet) != sf::Socket::Status::Done) {
		std::cerr << "Failed to send register data to server" << std::endl;
	}
	else {
		std::cout << "Register data sent to server: " << std::endl << username << std::endl << password << std::endl;
	}
}

void PacketManager::SendMatchmakeRequest(matchMode mode)
{
	hideAllButtons = true;

	sf::Packet packet;
	packetType type = MATCHMAKE;

	packet << type << mode;

	if (socket.send(packet) != sf::Socket::Status::Done) {
		std::cerr << "Failed to send matchmaking request to server" << std::endl;
	}
	else {
		std::cout << "Matchmaking request sent to server" << std::endl;
		std::cout << "Mode: " << (mode == COMPETITIVE ? "Competitive" : "NonCompetitive") << std::endl;
	}
}

void PacketManager::SendMapRequest() {
	sf::Packet packet;
	packetType type = MAP_REQUEST;

	packet << type;

	if (socket.send(packet) != sf::Socket::Status::Done) {
		std::cerr << "Failed to request map" << std::endl;
	}
}

// Procesa datos de ranking recibidos del servidor, extrae nombre, puntuación y posición de cada jugador y lo alamcena
void PacketManager::GetRanking(sf::Packet& data)
{
	ranking.clear();

	PlayerScore player;

	while (data >> player.name >> player.score >> player.position)
	{
		ranking.push_back(player);
	}
}

// Envía solicitud de ranking al servidor con el nombre del usuario actual
void PacketManager::RankingRequest() {
	sf::Packet packet;
	packetType type = RANKING;
	packet << type << myUsername;

	if (socket.send(packet) != sf::Socket::Status::Done) {
		std::cerr << "Failed to send ranking request to server" << std::endl;
	}
	else {
		std::cout << "Ranking request data sent to server: " << std::endl;
	}
}

// Procesa lista de P"P del servidor para establecer conexiones
void PacketManager::PeerListHandler(sf::Packet& data) {
	short myIdx, total;
	data >> myIdx >> total;

	myIndex = myIdx;
	totalPlayers = total;

	finishedCount = 0;
	for (unsigned short i = 0; i < 4; i++) finalRanking[i] = -1;
	for (unsigned short i = 0; i < 4; i++) allUsernames[i] = "";
	allUsernames[myIndex] = myUsername;
	disconnectedPlayers.clear();
	while (!pendingDisconnects.empty()) pendingDisconnects.pop();

	for (unsigned short i = 0; i < total; i++) {
		std::string ip;
		unsigned short port;
		short peerIdx;
		data >> ip >> port >> peerIdx;

		if (peerIdx >= myIndex) continue;

		if (peerIdx < myIndex) {
			sf::TcpSocket* s = new sf::TcpSocket();
			if (s->connect(sf::IpAddress::resolve(ip).value(), port) == sf::Socket::Status::Done) {
				s->setBlocking(false);
				sf::Packet hello;
				hello << myIndex;
				if (s->send(hello) == sf::Socket::Status::Done) {
					peerSockets[peerIdx] = s;
					std::cout << "Connected to peer " << peerIdx << " at " << ip << ":" << port << std::endl;
				}
				else {
					delete s;
					std::cerr << "Failed to send hello packet to peer " << peerIdx << std::endl;
				}
			}
			else {
				delete s;
				std::cerr << "Failed to connect to peer " << peerIdx << std::endl;
			}
		}
	}
}

// Envía un paquete a todos los jugadores conectados, itera sobre todos los sockets de lso juagadores e intenta enviar el paquete
void PacketManager::SendToPeers(sf::Packet& packet) {
	for (std::map<short, sf::TcpSocket*>::iterator it = peerSockets.begin(); it != peerSockets.end(); ++it) {
		if (it->second->send(packet) == sf::Socket::Status::Done) {
			std::cout << "Sent packet to peer " << it->first << std::endl;
		}
		else {
			std::cerr << "Failed to send packet to peer " << it->first << std::endl;
		}
	}
}

// Extrae y retorna la siguiente acción de juego pendiente de la cola, contiene el índice del jugador que envió la acción y el paquete con datos
std::pair<short, sf::Packet> PacketManager::PopPendingAction() {
	std::pair<short, sf::Packet> front = pendingActions.front();
	pendingActions.pop();
	return front;
}

// Extrae y retorna la siguiente información de jugador pendiente de la cola, contiene el índice del jugador y su información (usuario y puntuación)
std::pair<short, PlayerInfo> PacketManager::PopPendingPlayerInfo() {
	std::pair<short, PlayerInfo> front = pendingPlayerInfo.front();
	pendingPlayerInfo.pop();
	return front;
}

// Desconecta todos los jugadores P2P activos, libera memoria de sockets y limpia estructuras de datos P2P
void PacketManager::DisconnectPeers()
{
	for (std::pair<const short, sf::TcpSocket*>& entry : peerSockets) {
		entry.second->disconnect();
		delete entry.second;
	}
	peerSockets.clear();

	for (sf::TcpSocket* s : pendingAccepts) {
		s->disconnect();
		delete s;
	}
	pendingAccepts.clear();

	p2pReady = false;

	std::cout << "P2P peers disconnected successfully." << std::endl;
}

// Envía notificación de victoria a todos los jugadores, incluye el ID del jugador que ganó la partida
void PacketManager::SendWinNotificationToAll(short idPlayer)
{
	sf::Packet packet;
	packetType type = WIN_NOTIFICATION;
	packet << type << idPlayer;
	SendToPeers(packet);
	std::cout << "Win notification sent to peers: " << idPlayer << std::endl;
}

// Registra a un jugador como finalizado en la partida, almacena su ID en el ranking final y incrementa contador
void PacketManager::RecordWinner(short playerID)
{
	if (HasFinished(playerID)) return;
	finalRanking[finishedCount] = playerID;
	finishedCount++;
	std::cout << "Player " << playerID << " finished in position " << finishedCount << std::endl;
}

// Extrae y retorna el siguiente ID de jugador desconectado de la cola
short PacketManager::PopPendingDisconnect()
{
	short id = pendingDisconnects.front();
	pendingDisconnects.pop();
	return id;
}

// Envía resultado final de la partida al servidor, incluye ID de sala, ranking final y nombres de jugadores
void PacketManager::SendGameResult()
{
	sf::Packet packet;
	packetType type = GAME_RESULT;
	packet << type << currentLobbyId << finishedCount;
	std::cout << "SendGameResult: lobby=" << currentLobbyId << " finishedCount=" << finishedCount << std::endl;
	for (unsigned short i = 0; i < finishedCount; i++) {
		std::cout << "  [" << i << "] playerID=" << finalRanking[i] << " username=" << allUsernames[finalRanking[i]] << std::endl;
		packet << allUsernames[finalRanking[i]];
	}
	if (socket.send(packet) != sf::Socket::Status::Done) {
		std::cerr << "Failed to send game result" << std::endl;
	}
	else {
		std::cout << "Game result sent for lobby " << currentLobbyId << std::endl;
	}
}

// Libera todos los recursos del gestor de paquetes, desconecta del servidor y de todos los jugadores, libera memoria de todos los sockets
void PacketManager::Release() {
	if (serverConnected) {
		DisconnectFromServer();
	}
	for (std::pair<const short, sf::TcpSocket*>& entry : peerSockets) {
		entry.second->disconnect();
		delete entry.second;
	}
	peerSockets.clear();
	for (sf::TcpSocket* s : pendingAccepts) {
		s->disconnect();
		delete s;
	}
	pendingAccepts.clear();
}




