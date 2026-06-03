#include "PacketManager.h"
#include "SceneManager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
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

sf::Packet& operator <<(sf::Packet& packet, matchFinishReason reason) {
	return packet << static_cast<short>(reason);
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

sf::Packet& operator >>(sf::Packet& packet, matchFinishReason& reason) {
	short  temp;
	packet >> temp;
	reason = static_cast<matchFinishReason>(temp);
	return packet;
}

// Establece conexi�n TCP con el servidor central, desconecta primero si ya hay una conexi�n activa
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

// Cierra la conexi�n con el servidor, reinicia el socket y actualiza el estado de conexi�n
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

	if (udpConnected) {
		char buffer[1024];
		std::size_t receivedSize;
		std::optional<sf::IpAddress> senderIP;
		unsigned short senderPort;

		while (udpSocket.receive(buffer, sizeof(buffer), receivedSize, senderIP, senderPort) == sf::Socket::Status::Done) {
			std::size_t readPos = 0;

			udpPacketType packetType;
			std::memcpy(&packetType, buffer + readPos, sizeof(packetType));
			readPos += sizeof(packetType);

			lastUdpPacketTime = udpClock.getElapsedTime().asSeconds();

			switch (packetType) {
			case MOVEMENT:
				HandleMovement(buffer, receivedSize, readPos);
				break;

			case SHOOT:
				HandleShoot(buffer, receivedSize, readPos);
				break;

			case HIT:
				HandleHit(buffer, receivedSize, readPos);
				break;

			case TAUNT:
				HandleTaunt(buffer, receivedSize, readPos);
				break;

			case PLAYER_HEALTH_UPDATE:
				HandleEnemyHealthUpdate(buffer, receivedSize, readPos);
				break;

			case PING:
				HandlePing(buffer, receivedSize, readPos);
				break;

			case PONG:
				HandlePong(buffer, receivedSize, readPos);
				break;

			case DISCONNECTED_PLAYER:
				HandleDisconnectedPlayer(buffer, receivedSize, readPos);
				break;

			case IRREGULARITY_WARNING:
				HandleIrregularityWarning(buffer, receivedSize, readPos);
				break;

			case MATCH_FINISHED:
				HandleMatchFinished(buffer, receivedSize, readPos);
				break;

			case SHOOT_CONFIRMED:
				HandleShootConfirmed(buffer, receivedSize, readPos);
				break;

			default:
				break;
			}
		}
	}

	if (udpConnected) {
		UpdatePingSystem();
	}
}


// Libera todos los recursos del gestor de paquetes, desconecta del servidor y de todos los jugadores, libera memoria de todos los sockets
void PacketManager::Release() {
	if (serverConnected) {
		DisconnectFromServer();
	}
}

// Paquete de handshake del servidor
void PacketManager::HandShake(sf::Packet& data) {
	std::string reciveMessage;
	data >> reciveMessage;
	std::cout << "Message from server: " << reciveMessage << std::endl;
}

// Respuesta de login del servidor, extrae credenciales, resultado de autenticaci�n y puntuaci�n
void PacketManager::Login(sf::Packet& data) {
	std::string user;
	std::string pass;
	authResult result;
	int score = 0;
	unsigned short myPlayerIndex = 0;

	data >> user;
	data >> pass;
	data >> result;
	data >> score;
	data >> myPlayerIndex;


	playerInfo.username = user;
	playerInfo.score = score;
	myIndex = myPlayerIndex;


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

// Respuesta de registro del servidor, extrae credenciales y resultado de autenticaci�n
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
		SM.SetNextScene("Gameplay");
		if(udpSocket.bind(sf::Socket::AnyPort) == sf::Socket::Status::Done) {	//Hecho con IA. Bindeamos el Puerto a cualquier puerto. Al enviar mensajes si que usaremos
			udpSocket.setBlocking(false);										//el UDP_SERVER_PORT, pero con esto hacemos que el cliente no tenga conflicto
			udpConnected = true;												//al bindear el puerto en el mismo PC al usar varios clientes.
			lastUdpPacketTime = udpClock.getElapsedTime().asSeconds();
			lastPingTime = lastUdpPacketTime;
			waitingPong = false;
			lastPingId = 0;
		}
		else {
			std::cerr << "Failed to bind UDP socket to port " << sf::Socket::AnyPort << std::endl;
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
		unsigned int lastProcessedMovementID;
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
			// Validacion Local Player
			LocalValidation validation;
			validation.movementId = lastProcessedMovementID;
			validation.position = Vector2(x, y);

			pendingLocalValidations.push(validation);
		}
		else {
			// Interpolacion Online Player
			OnlineMovement movement;
			movement.movementId = lastProcessedMovementID;
			movement.position = Vector2(x, y);

			pendingOnlineMovements.push(movement);
		}
	}
}

void PacketManager::HandleEnemyHealthUpdate(const char* buffer, std::size_t receivedSize, std::size_t readPos) {
	EnemyHealthUpdate update;

	if (readPos + sizeof(update.playerId) + sizeof(update.lives) + sizeof(update.health) > receivedSize) {
		return;
	}

	std::memcpy(&update.playerId, buffer + readPos, sizeof(update.playerId));
	readPos += sizeof(update.playerId);

	std::memcpy(&update.lives, buffer + readPos, sizeof(update.lives));
	readPos += sizeof(update.lives);

	std::memcpy(&update.health, buffer + readPos, sizeof(update.health));
	readPos += sizeof(update.health);

	pendingEnemyHealthUpdates.push(update);
}

// Envia solicitud de login al servidor con usuario y contrase�a
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

// Env�a solicitud de registro al servidor con usuario y contrase�a
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

// Procesa datos de ranking recibidos del servidor, extrae nombre, puntuaci�n y posici�n de cada jugador y lo alamcena
void PacketManager::GetRanking(sf::Packet& data)
{
	ranking.clear();

	PlayerScore player;

	while (data >> player.name >> player.score >> player.position)
	{
		ranking.push_back(player);
	}
}

// Env�a solicitud de ranking al servidor con el nombre del usuario actual
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
	char flags = 0;

	std::memcpy(buffer + bufferDataSize, &flags, sizeof(flags));
	bufferDataSize += sizeof(flags);

	std::memcpy(buffer + bufferDataSize, &packetType, sizeof(packetType));
	bufferDataSize += sizeof(packetType);

	std::memcpy(buffer + bufferDataSize, &myIndex, sizeof(myIndex));
	bufferDataSize += sizeof(myIndex);

	std::memcpy(buffer + bufferDataSize, &movementType, sizeof(movementType));
	bufferDataSize += sizeof(movementType);

	std::memcpy(buffer + bufferDataSize, &movementID, sizeof(movementID));
	bufferDataSize += sizeof(movementID);

	std::memcpy(buffer + bufferDataSize, &x, sizeof(x));
	bufferDataSize += sizeof(x);

	std::memcpy(buffer + bufferDataSize, &y, sizeof(y));
	bufferDataSize += sizeof(y);

	if (udpSocket.send(buffer, bufferDataSize, UDP_SERVER_IP, UDP_SERVER_PORT) != sf::Socket::Status::Done) {
		std::cerr << "Failed to send movement data to server" << std::endl;
	}
}

PacketManager::OnlineMovement PacketManager::PopPendingOnlineMovement() {
	OnlineMovement movement = pendingOnlineMovements.front();
	pendingOnlineMovements.pop();
	return movement;
}

PacketManager::LocalValidation PacketManager::PopPendingLocalValidation() {
	LocalValidation validation = pendingLocalValidations.front();
	pendingLocalValidations.pop();
	return validation;
}

void PacketManager::SendLifeHealthUpdate(short lives, short health) {
	char buffer[1024];
	std::size_t size = 0;

	udpPacketType packetType = PLAYER_HEALTH_UPDATE;
	char flags = 0;
	unsigned short clientId = myIndex;

	std::memcpy(buffer + size, &flags, sizeof(flags));
	size += sizeof(flags);

	std::memcpy(buffer + size, &packetType, sizeof(packetType));
	size += sizeof(packetType);

	std::memcpy(buffer + size, &clientId, sizeof(clientId));
	size += sizeof(clientId);

	std::memcpy(buffer + size, &lives, sizeof(lives));
	size += sizeof(lives);

	std::memcpy(buffer + size, &health, sizeof(health));
	size += sizeof(health);

	if (udpSocket.send(buffer, size, UDP_SERVER_IP, UDP_SERVER_PORT) != sf::Socket::Status::Done) {
		std::cerr << "Failed to send PLAYER_HEALTH_UPDATE packet" << std::endl;
	}
}

PacketManager::EnemyHealthUpdate PacketManager::PopPendingEnemyHealthUpdate() {
	EnemyHealthUpdate update = pendingEnemyHealthUpdates.front();
	pendingEnemyHealthUpdates.pop();
	return update;
}

void PacketManager::SendPing() {
	char buffer[1024];
	std::size_t size = 0;

	udpPacketType packetType = PING;
	char flags = 0;
	unsigned short clientId = myIndex;
	unsigned int pingId = lastPingId + 1;

	std::memcpy(buffer + size, &flags, sizeof(flags));
	size += sizeof(flags);

	std::memcpy(buffer + size, &packetType, sizeof(packetType));
	size += sizeof(packetType);

	std::memcpy(buffer + size, &clientId, sizeof(clientId));
	size += sizeof(clientId);

	std::memcpy(buffer + size, &pingId, sizeof(pingId));
	size += sizeof(pingId);

	if (udpSocket.send(buffer, size, UDP_SERVER_IP, UDP_SERVER_PORT) == sf::Socket::Status::Done) {
		lastPingId = pingId;
		lastPingTime = udpClock.getElapsedTime().asSeconds();
		waitingPong = true;
	}
}

void PacketManager::SendPong(unsigned int pingId) {
	char buffer[1024];
	std::size_t size = 0;

	udpPacketType packetType = PONG;
	char flags = 0;
	unsigned short clientId = myIndex;

	std::memcpy(buffer + size, &flags, sizeof(flags));
	size += sizeof(flags);

	std::memcpy(buffer + size, &packetType, sizeof(packetType));
	size += sizeof(packetType);

	std::memcpy(buffer + size, &clientId, sizeof(clientId));
	size += sizeof(clientId);

	std::memcpy(buffer + size, &pingId, sizeof(pingId));
	size += sizeof(pingId);

	if(udpSocket.send(buffer, size, UDP_SERVER_IP, UDP_SERVER_PORT) != sf::Socket::Status::Done) {
		std::cerr << "Failed to send pong data to server" << std::endl;
	}
}

void PacketManager::HandlePing(const char* buffer, std::size_t receivedSize, std::size_t readPos) {
	unsigned short clientId = 0;
	unsigned int pingId = 0;

	std::memcpy(&clientId, buffer + readPos, sizeof(clientId));
	readPos += sizeof(clientId);

	std::memcpy(&pingId, buffer + readPos, sizeof(pingId));
	readPos += sizeof(pingId);

	if (clientId != myIndex)
		return;

	lastUdpPacketTime = udpClock.getElapsedTime().asSeconds();
	waitingPong = false;

	SendPong(pingId);
}

void PacketManager::HandlePong(const char* buffer, std::size_t receivedSize, std::size_t readPos) {
	unsigned short clientId = 0;
	unsigned int pingId = 0;

	std::memcpy(&clientId, buffer + readPos, sizeof(clientId));
	readPos += sizeof(clientId);

	std::memcpy(&pingId, buffer + readPos, sizeof(pingId));
	readPos += sizeof(pingId);

	if (clientId != myIndex || pingId != lastPingId)
		return;

	lastUdpPacketTime = udpClock.getElapsedTime().asSeconds();
	waitingPong = false;
}

void PacketManager::UpdatePingSystem() {
	const float PING_THRESHOLD = 1.0f;
	const float PING_INTERVAL = 0.5f;
	const float TIMEOUT = 3.0f;

	float currentTime = udpClock.getElapsedTime().asSeconds();
	float timeSinceLastPacket = currentTime - lastUdpPacketTime;

	if (timeSinceLastPacket >= TIMEOUT) {
		std::cout << "UDP server timeout" << std::endl;
		udpConnected = false;
		waitingPong = false;
		processedShootIds.clear();
	pendingShootConfirmedCount = 0;
		udpSocket.unbind();
		SM.SetNextScene("Lobby");
		return;
	}

	if (timeSinceLastPacket >= PING_THRESHOLD) {
		if (!waitingPong || currentTime - lastPingTime >= PING_INTERVAL) {
			SendPing();
		}
	}
}

void PacketManager::HandleDisconnectedPlayer(const char* buffer, std::size_t receivedSize, std::size_t readPos) {
	unsigned short disconnectedClientId = 0;

	std::memcpy(&disconnectedClientId, buffer + readPos, sizeof(disconnectedClientId));
	readPos += sizeof(disconnectedClientId);

	disconnectedPlayers.push_back(disconnectedClientId);

	std::cout << "Player disconnected: " << disconnectedClientId << std::endl;

	udpConnected = false;
	waitingPong = false;
	processedShootIds.clear();
	pendingShootConfirmedCount = 0;
	udpSocket.unbind();

	SM.SetNextScene("Lobby");
}

void PacketManager::HandleIrregularityWarning(const char* buffer, std::size_t receivedSize, std::size_t readPos) {
	unsigned short clientId = 0;
	unsigned int movementID = 0;
	float validX = 0.0f;
	float validY = 0.0f;
	unsigned short irregularityCount = 0;

	std::memcpy(&clientId, buffer + readPos, sizeof(clientId));
	readPos += sizeof(clientId);

	std::memcpy(&movementID, buffer + readPos, sizeof(movementID));
	readPos += sizeof(movementID);

	std::memcpy(&validX, buffer + readPos, sizeof(validX));
	readPos += sizeof(validX);

	std::memcpy(&validY, buffer + readPos, sizeof(validY));
	readPos += sizeof(validY);

	std::memcpy(&irregularityCount, buffer + readPos, sizeof(irregularityCount));
	readPos += sizeof(irregularityCount);

	if (clientId == myIndex) {
		LocalValidation validation;
		validation.movementId = movementID;
		validation.position = Vector2(validX, validY);
		pendingLocalValidations.push(validation);
	}

	std::cout << "Irregularidad detectada: " << irregularityCount << "/3" << std::endl;
}

void PacketManager::SendHit() {
	char buffer[1024];
	std::size_t size = 0;

	udpPacketType packetType = HIT;
	uint8_t flags = URGENT_PACKET;

	std::memcpy(buffer + size, &flags, sizeof(flags));
	size += sizeof(flags);

	std::memcpy(buffer + size, &packetType, sizeof(packetType));
	size += sizeof(packetType);

	std::memcpy(buffer + size, &myIndex, sizeof(myIndex));
	size += sizeof(myIndex);

	if (udpSocket.send(buffer, size, UDP_SERVER_IP, UDP_SERVER_PORT) != sf::Socket::Status::Done) {
		std::cerr << "Failed to send hit packet" << std::endl;
	}
}

void PacketManager::HandleHit(const char* buffer, std::size_t receivedSize, std::size_t readPos) {
	unsigned short targetPlayerId = 0;
	std::memcpy(&targetPlayerId, buffer + readPos, sizeof(targetPlayerId));

	HitConfirmedData hitData;
	hitData.targetPlayerId = targetPlayerId;
	pendingHitConfirmations.push(hitData);
}

PacketManager::HitConfirmedData PacketManager::PopPendingHitConfirmation() {
	HitConfirmedData hitData = pendingHitConfirmations.front();
	pendingHitConfirmations.pop();
	return hitData;
}

void PacketManager::SendTaunt()
{
	char buffer[1024];
	std::size_t size = 0;

	udpPacketType packetType = TAUNT;
	char flags = 0;
	unsigned int tauntId = 0;

	std::memcpy(buffer + size, &flags, sizeof(flags));
	size += sizeof(flags);

	std::memcpy(buffer + size, &packetType, sizeof(packetType));
	size += sizeof(packetType);

	std::memcpy(buffer + size, &myIndex, sizeof(myIndex));
	size += sizeof(myIndex);

	std::memcpy(buffer + size, &tauntId, sizeof(tauntId));
	size += sizeof(tauntId);

	if (udpSocket.send(buffer, size, UDP_SERVER_IP, UDP_SERVER_PORT) != sf::Socket::Status::Done) {
		std::cout << "Failed to send taunt packet" << std::endl;
	}
}

void PacketManager::HandleTaunt(const char* buffer, std::size_t receivedSize, std::size_t readPos)
{
	unsigned short clientId = 0;
	unsigned int tauntId = 0;

	std::memcpy(&clientId, buffer + readPos, sizeof(clientId));
	readPos += sizeof(clientId);

	std::memcpy(&tauntId, buffer + readPos, sizeof(tauntId));
	readPos += sizeof(tauntId);

	if (clientId == myIndex) {
		return;
	}

	AUDIO->PlayClip("taunt", 0, 128);
	pendingTauntCount++;
}

void PacketManager::SendShoot(float spawnX, float spawnY, float directionX, float directionY) {
	char buffer[1024];
	std::size_t size = 0;

	udpPacketType packetType = SHOOT;
	uint8_t flags = URGENT_PACKET | CRITIC_PACKET;

	std::memcpy(buffer + size, &flags, sizeof(flags));
	size += sizeof(flags);

	std::memcpy(buffer + size, &packetType, sizeof(packetType));
	size += sizeof(packetType);

	std::memcpy(buffer + size, &myIndex, sizeof(myIndex));
	size += sizeof(myIndex);

	if (udpSocket.send(buffer, size, UDP_SERVER_IP, UDP_SERVER_PORT) != sf::Socket::Status::Done) {
		std::cerr << "Failed to send shoot packet" << std::endl;
	}
}

void PacketManager::HandleShoot(const char* buffer, std::size_t receivedSize, std::size_t readPos) {
	unsigned short shooterNetworkId = 0;
	unsigned short criticalPacketId = 0;

	if (readPos + sizeof(shooterNetworkId) + sizeof(criticalPacketId) > receivedSize)
		return;

	std::memcpy(&shooterNetworkId, buffer + readPos, sizeof(shooterNetworkId));
	readPos += sizeof(shooterNetworkId);

	std::memcpy(&criticalPacketId, buffer + readPos, sizeof(criticalPacketId));
	readPos += sizeof(criticalPacketId);

	SendShootAck(criticalPacketId);

	if (shooterNetworkId == myIndex) return;

	if (processedShootIds.count(criticalPacketId) > 0) return;
	processedShootIds.insert(criticalPacketId);

	ShootData shootData;
	shootData.shooterNetworkId = shooterNetworkId;

	pendingShoots.push(shootData);
}

void PacketManager::HandleShootConfirmed(const char* buffer, std::size_t receivedSize, std::size_t readPos) {
	unsigned short shooterNetworkId = 0;

	if (readPos + sizeof(shooterNetworkId) > receivedSize)
		return;

	std::memcpy(&shooterNetworkId, buffer + readPos, sizeof(shooterNetworkId));
	readPos += sizeof(shooterNetworkId);

	if (shooterNetworkId != myIndex) return;

	pendingShootConfirmedCount++;
}

void PacketManager::SendShootAck(unsigned short criticalPacketId) {
	char buffer[1024];
	std::size_t size = 0;

	udpPacketType packetType = SHOOT_ACK;
	uint8_t flags = URGENT_PACKET;

	std::memcpy(buffer + size, &flags, sizeof(flags));
	size += sizeof(flags);

	std::memcpy(buffer + size, &packetType, sizeof(packetType));
	size += sizeof(packetType);

	std::memcpy(buffer + size, &myIndex, sizeof(myIndex));
	size += sizeof(myIndex);

	std::memcpy(buffer + size, &criticalPacketId, sizeof(criticalPacketId));
	size += sizeof(criticalPacketId);

	if (udpSocket.send(buffer, size, UDP_SERVER_IP, UDP_SERVER_PORT) != sf::Socket::Status::Done) {
		std::cerr << "Failed to send SHOOT_ACK packet" << std::endl;
	}
}

PacketManager::ShootData PacketManager::PopPendingShoot() {
	ShootData shootData = pendingShoots.front();
	pendingShoots.pop();
	return shootData;
}

void PacketManager::HandleMatchFinished(const char* buffer, std::size_t receivedSize, std::size_t readPos) {
	unsigned short resultValue = 0;
	unsigned short reasonValue = 0;

	if (readPos + sizeof(resultValue) + sizeof(reasonValue) > receivedSize) {
		return;
	}

	std::memcpy(&resultValue, buffer + readPos, sizeof(resultValue));
	readPos += sizeof(resultValue);

	std::memcpy(&reasonValue, buffer + readPos, sizeof(reasonValue));
	readPos += sizeof(reasonValue);

	lastMatchResult = static_cast<matchResult>(resultValue);
	lastMatchFinishReason = static_cast<matchFinishReason>(reasonValue);
	matchFinishedReceived = true;

	if (lastMatchResult == MATCH_RESULT_WIN) {
		std::cout << "HAS GANADO" << std::endl;
	}
	else {
		std::cout << "HAS PERDIDO" << std::endl;
	}

	if (lastMatchFinishReason == FINISH_BY_LIVES) {
		std::cout << "Motivo: FINISH_BY_LIVES" << std::endl;
	}
	else if (lastMatchFinishReason == FINISH_BY_DISCONNECT) {
		std::cout << "Motivo: FINISH_BY_DISCONNECT" << std::endl;
	}
	else if (lastMatchFinishReason == FINISH_BY_IRREGULARITY) {
		std::cout << "Motivo: FINISH_BY_IRREGULARITY" << std::endl;
	}

	udpConnected = false;
	waitingPong = false;
	processedShootIds.clear();
	pendingShootConfirmedCount = 0;
	udpSocket.unbind();
}
