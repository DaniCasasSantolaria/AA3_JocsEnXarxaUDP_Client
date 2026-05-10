#include "PacketManager.h"
#include "SceneManager.h"

#include <iostream>

sf::Packet& operator <<(sf::Packet& packet, packetType type) {
	return packet << static_cast<short>(type);
}

sf::Packet& operator <<(sf::Packet& packet, authResult result) {
	return packet << static_cast<short>(result);
}

sf::Packet& operator <<(sf::Packet& packet, lobbyResult result) {
	return packet << static_cast<short>(result);
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

sf::Packet& operator >>(sf::Packet& packet, lobbyResult& result) {
	short  temp;
	packet >> temp;
	result = static_cast<lobbyResult>(temp);
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
			case CREATE_LOBBY:
				CreateLobby(packet);
				break;
			case JOIN_LOBBY:
				JoinLobby(packet);
				break;
			case RANKING:
				GetRanking(packet);
				break;
			case OPEN_LISTENER:
				OpenListenerHandler(packet);
				break;
			case PEER_LIST:
				PeerListHandler(packet);
				break;
			default:
				break;
			}
			packet.clear();
		}
	}

	// Aceptar conexiones P2P
	if (!p2pReady) {
		if (myListener.getLocalPort() != 0)
		{
			sf::TcpSocket* incoming = new sf::TcpSocket();
			if (myListener.accept(*incoming) == sf::Socket::Status::Done) {
				incoming->setBlocking(false);
				pendingAccepts.push_back(incoming);
			}
			else {
				delete incoming;
			}
		}
	}

	// Identificar conexiones P2P entrantes
	for (std::vector<sf::TcpSocket*>::iterator it = pendingAccepts.begin(); it != pendingAccepts.end(); ) {
		sf::Packet hello;
		if ((*it)->receive(hello) == sf::Socket::Status::Done) {
			short peerIdx;
			hello >> peerIdx;
			peerSockets[peerIdx] = *it;
			std::cout << "Peer " << peerIdx << " identified (incoming connection)" << std::endl;
			it = pendingAccepts.erase(it);
		}
		else {
			++it;
		}
	}

	// Verificar si P2P está listo
	if (!p2pReady && totalPlayers > 0
		&& peerSockets.size() == totalPlayers - 1
		&& pendingAccepts.empty())
	{
		p2pReady = true;
		DisconnectFromServer();
		std::cout << "P2P mesh ready. Own index: " << myIndex << std::endl;
		SM.SetNextScene("Gameplay");
	}

	// Recibir paquetes de P2P
	if (p2pReady) {
		std::vector<short> disconnectedPeers;

		for (std::pair<const short, sf::TcpSocket*>& entry : peerSockets) {
			sf::Packet p;
			sf::Socket::Status status = entry.second->receive(p);
			if (status == sf::Socket::Status::Done) {
				packetType type;
				p >> type;
				if (type == TURN_ACTION) {
					pendingActions.push({ entry.first, p });
				}
				else if (type == PLAYER_INFO) {
					PlayerInfo info;
					p >> info.username >> info.score;
					allUsernames[entry.first] = info.username;
					pendingPlayerInfo.push({ entry.first, info });
				}
				else if (type == WIN_NOTIFICATION) {
					short idPlayer;
					p >> idPlayer;
					std::cout << "Win notification received from player " << idPlayer << std::endl;
				}
				else {
					std::cout << "Received unknown packet type from peer " << entry.first << std::endl;
				}
			}
			else if (status == sf::Socket::Status::Disconnected) {
				std::cout << "Peer " << entry.first << " disconnected" << std::endl;
				disconnectedPeers.push_back(entry.first);
				disconnectedPlayers.push_back(entry.first);
				pendingDisconnects.push(entry.first);
				delete entry.second;
				break;
			}
		}

		for (short i : disconnectedPeers) {
			peerSockets.erase(i);
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

// Respuesta de creación de sala del servidor, extrae ID de sala y resultado de operación
void PacketManager::CreateLobby(sf::Packet& data) {
	std::string idLobby;
	lobbyResult result;

	data >> idLobby;
	data >> result;

	switch (result) {
	case LOBBY_CREATED_OK:
		currentLobbyId = idLobby;
		hideAllButtons = true;
		std::cout << "Lobby created: " << idLobby << std::endl;
		break;
	case LOBBY_ALREADY_EXISTS:
		std::cout << "Lobby ID already exists" << std::endl;
		break;
	default:
		std::cout << "Error creating lobby" << std::endl;
		break;
	}
}

// Respuesta de entrada a sala del servidor, extrae ID de sala y resultado de operación
void PacketManager::JoinLobby(sf::Packet& data) {
	std::string idLobby;
	lobbyResult result;

	data >> idLobby;
	data >> result;

	std::cout << "Lobby ID: " << idLobby << std::endl;

	switch (result) {
	case LOBBY_JOINED_OK:
		currentLobbyId = idLobby;
		hideAllButtons = true;
		std::cout << "Joined lobby: " << idLobby << std::endl;
		break;
	case LOBBY_FULL:
		std::cout << "Lobby is full" << std::endl;
		break;
	case LOBBY_NOT_FOUND:
		std::cout << "Lobby not found" << std::endl;
		break;
	default:
		std::cout << "Error joining lobby" << std::endl;
		break;
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

// Envía solicitud de creación de sala al servidor con un ID de sala específico
void PacketManager::CreateActionRequest(const std::string& idLobby) {
	sf::Packet packet;
	packetType type = CREATE_LOBBY;
	packet << type << idLobby;

	if (socket.send(packet) != sf::Socket::Status::Done) {
		std::cerr << "Failed to send create lobby data to server" << std::endl;
	}
	else {
		std::cout << "Create lobby data sent to server" << std::endl << idLobby << std::endl;
	}
}

// Envía solicitud de entrada a sala al servidor con un ID de sala específico
void PacketManager::JoinActionRequest(const std::string& idLobby) {
	sf::Packet packet;
	packetType type = JOIN_LOBBY;
	packet << type << idLobby;

	if (socket.send(packet) != sf::Socket::Status::Done) {
		std::cerr << "Failed to send join lobby data to server" << std::endl;
	}
	else {
		std::cout << "Join lobby data sent to server: " << std::endl;
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

// Procesa instrucción del servidor para abrir listener P2P, extrae índice de cliente y ID de sala
void PacketManager::OpenListenerHandler(sf::Packet& data) {
	short index;
	std::string lobbyId;
	data >> index >> lobbyId;

	myIndex = index;
	currentLobbyId = lobbyId;

	myListener.setBlocking(false);
	myListener.listen(0);

	unsigned short port = myListener.getLocalPort();

	sf::Packet response;
	response << CLIENT_PORT << lobbyId << port;
	if (socket.send(response) == sf::Socket::Status::Done) {
		std::cout << "Listener opened on port " << port << ", index=" << myIndex << std::endl;
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

// Envía información del jugador actual (usuario y puntuación) a todos los jugadores
void PacketManager::SendPlayerInfoToAll() {
	sf::Packet packet;
	packetType type = PLAYER_INFO;
	std::string username = playerInfo.username;
	int score = playerInfo.score;
	packet << type << username << score;
	SendToPeers(packet);
	std::cout << "Player info sent to peers: " << username << " with score: " << score << std::endl;
}

// Envía saludo inicial a todos los jugadores
void PacketManager::SendGreetingToAll() {
	sf::Packet packet;
	packetType type = PLAYER_INFO;
	std::string greeting = "Hola: " + myUsername;
	packet << type << greeting;
	SendToPeers(packet);
	std::cout << "Greeting sent to peers: " << greeting << std::endl;
}

// Extrae y retorna la siguiente información de jugador pendiente de la cola, contiene el índice del jugador y su información (usuario y puntuación)
std::pair<short, PlayerInfo> PacketManager::PopPendingPlayerInfo() {
	std::pair<short, PlayerInfo> front = pendingPlayerInfo.front();
	pendingPlayerInfo.pop();
	return front;
}

// Envía acción de turno (movimiento) a todos los jugadores, incluye fila, columna e ID del jugador que realiza la acción
void PacketManager::SendTurnAction(short row, short col, short playerID) {
	sf::Packet packet;
	packetType type = TURN_ACTION;
	packet << type << row << col << playerID;
	SendToPeers(packet);
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