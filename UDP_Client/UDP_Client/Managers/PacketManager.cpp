#include "PacketManager.h"
#include "SceneManager.h"

#include <iostream>
#include <fstream>
#include <sstream>
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

sf::Packet& operator<<(sf::Packet& packet, movementPacketType status) {
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

sf::Packet& operator>>(sf::Packet& packet, movementPacketType& status) {
	short  temp;
	packet >> temp;
	status = static_cast<movementPacketType>(temp);
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
			case MAP_REQUEST:
				HandleMapRequest(packet);
				break;
			default:
				break;
			}
			packet.clear();
		}
	}

	if(udpConnected) {
		char buffer[1024];
		std::size_t receivedSize;
		std::optional <sf::IpAddress> senderIP;
		unsigned short senderPort;
		if (udpSocket.receive(buffer, sizeof(buffer), receivedSize, senderIP, senderPort) == sf::Socket::Status::Done) {
			std::size_t readPos = 0;

			udpPacketType packetType;
			std::memcpy(&packetType, buffer + readPos, sizeof(packetType));
			readPos += sizeof(packetType);

			switch (packetType) {
			case udpPacketType::MOVEMENT:
				HandleMovement(buffer, receivedSize, readPos);
				break;

			default:
				break;
			}
		}
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
	bool matchStarted = false;
	switch (mode)
	{
	case COMPETITIVE:
		if (status == QUEUE_WAITING) {
			std::cout << "Added to competitive matchmaking queue. Waiting for match..." << std::endl;
		}
		else {
			std::cout << "Competitive match found. Opening UDP socket..." << std::endl;
			matchStarted = true;
		}
		break;
	case NON_COMPETITIVE:
		if(status == QUEUE_WAITING) {
			std::cout << "Added to non-competitive matchmaking queue. Waiting for match..." << std::endl;
		}
		else {
			std::cout << "Non-competitive match found. Opening UDP socket..." << std::endl;
			matchStarted = true;
		}
		break;
	default:
		std::cout << "Unknown matchmaking mode" << std::endl;
		break;
	}

	if (matchStarted) {
		if(udpSocket.bind(sf::Socket::AnyPort) == sf::Socket::Status::Done) {
			udpSocket.setBlocking(false);
			udpConnected = true;
		}
		else {
			std::cerr << "Failed to bind UDP socket to port " << UDP_SERVER_PORT << std::endl;
		}
	}
}

void PacketManager::HandleMapRequest(sf::Packet& packet) {
	short requestTypeValue;
	packet >> requestTypeValue;

	mapRequestType requestType = static_cast<mapRequestType>(requestTypeValue);

	if (requestType == MAP_UP_TO_DATE) {
		unsigned short serverVersion;
		packet >> serverVersion;

		std::cout << "Map already updated. Actual version: " << serverVersion << std::endl;
		return;
	}
	else if (requestType == MAP_UPDATE) {
		unsigned short serverVersion;
		std::string mapContent;

		packet >> serverVersion >> mapContent;

		SaveLocalMap(mapContent);
		SaveLocalMapVersion(serverVersion);

		std::cout << "Map updated to the new version: " << serverVersion << std::endl;
	}
}

void PacketManager::HandleMovement(const char* buffer, std::size_t receivedSize, std::size_t readPos) {
	movementPacketType movementType;
	std::memcpy(&movementType, buffer + readPos, sizeof(movementType));
	readPos += sizeof(movementType);

	if (movementType == movementPacketType::RECEIVE_VALIDATED_MOVEMENT) {
		unsigned short playerId;
		unsigned short lastProcessedMovementID;
		float x;
		float y;


		std::memcpy(&playerId, buffer + readPos, sizeof(playerId));
		readPos += sizeof(playerId);

		std::memcpy(&lastProcessedMovementID, buffer + readPos, sizeof(lastProcessedMovementID));
		readPos += sizeof(lastProcessedMovementID);

		std::memcpy(&x, buffer + readPos, sizeof(x));
		readPos += sizeof(x);

		std::memcpy(&y, buffer + readPos, sizeof(y));
		readPos += sizeof(y);

		if (playerId == myIndex) {
			// Validation Local Player
		}
		else {
			// Interpolation Online Player
		}
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

void PacketManager::RequestMap() {
	unsigned short localVersion = LoadLocalMapVersion();

	sf::Packet packet;
	packet << MAP_REQUEST << static_cast<short>(MAP_VERSION_CHECK) << localVersion;

	if (socket.send(packet) != sf::Socket::Status::Done) {
		std::cerr << "Failed to request map" << std::endl;
	}
}

unsigned short PacketManager::LoadLocalMapVersion() {
	std::ifstream file("resources/Maps/map_version.txt");

	unsigned short version = 0;

	if (file.is_open()) {
		file >> version;
	}

	return version;
}

void PacketManager::SaveLocalMap(const std::string& mapContent) {
	std::ofstream file("resources/Maps/Map.txt");

	if (!file.is_open()) {
		std::cerr << "Map file could not be opened" << std::endl;
		return;
	}

	file << mapContent;
}

void PacketManager::SaveLocalMapVersion(unsigned short version) {
	std::ofstream file("resources/Maps/map_version.txt");

	if (!file.is_open()) {
		std::cerr << "Map file could not be opened" << std::endl;
		return;
	}

	file << version;
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

void PacketManager::SendMovement(float x, float y, unsigned int movementID) {
	char buffer[1024];
	std::size_t bufferDataSize = 0;

	udpPacketType packetType = udpPacketType::MOVEMENT;
	movementPacketType movementType = movementPacketType::SEND_RAW_MOVEMENT;

	std::memcpy(buffer + bufferDataSize, &packetType, sizeof(packetType));
	bufferDataSize += sizeof(packetType);

	std::memcpy(buffer + bufferDataSize, &movementType, sizeof(movementType));
	bufferDataSize += sizeof(movementType);

	std::memcpy(buffer + bufferDataSize, &movementID, sizeof(movementID));
	bufferDataSize += sizeof(movementID);

	std::memcpy(buffer + bufferDataSize, &x, sizeof(x));
	bufferDataSize += sizeof(x);

	std::memcpy(buffer + bufferDataSize, &y, sizeof(y));
	bufferDataSize += sizeof(y);

	if (udpSocket.send(buffer, bufferDataSize, UDP_SERVER_IP, UDP_SERVER_PORT) == sf::Socket::Status::Done) {
		std::cout << "Movement data sent to server: " << std::endl;
	}
	else {
		std::cerr << "Failed to send movement data to server" << std::endl;
	}
}