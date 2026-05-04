#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <map>
#include <vector>
#include <queue>
#include <utility>

#define PM PacketManager::Instance()

// Enum de tipos de paquetes que se pueden enviar/recibir
enum packetType { HANDSHAKE, LOGIN, REGISTER, RANKING, CREATE_LOBBY, JOIN_LOBBY, OPEN_LISTENER, CLIENT_PORT, PEER_LIST, TURN_ACTION, PLAYER_INFO, WIN_NOTIFICATION, GAME_RESULT };

// Enum de resultados posibles en autenticación
enum authResult { LOGIN_OK, USER_NOT_FOUND, WRONG_PASSWORD, REGISTER_OK, USER_ALREADY_EXISTS };

// Enum de resultados posibles en operaciones de sala
enum lobbyResult { LOBBY_CREATED_OK, LOBBY_ALREADY_EXISTS, LOBBY_CREATE_ERROR, LOBBY_JOINED_OK, LOBBY_NOT_FOUND, LOBBY_FULL };

// Información básica del jugador
struct PlayerInfo {
    std::string username;
    int score;
};

// Gestor de paquetes de red
// Responsable de manejar toda la comunicación TCP/IP del cliente
class PacketManager {
private:
    // Constantes de configuración de red
    unsigned const int LISTENER_PORT = 55007; // Port 
    const sf::IpAddress SERVER_IP = sf::IpAddress(10, 8, 0, 2); // IP

    // Sockets de comunicación
    sf::TcpSocket socket;
    sf::TcpListener myListener;
    std::map<int, sf::TcpSocket*> peerSockets;
    std::vector<sf::TcpSocket*> pendingAccepts;

    // Estado de conexión
    int myIndex = -1;
    int totalPlayers = 0;
    bool serverConnected = false;
    bool p2pReady = false;
    std::string currentLobbyId;
	    
    // Colas de eventos pendientes de procesar
    std::queue<std::pair<int, sf::Packet>> pendingActions;
    std::queue<std::pair<int, PlayerInfo>> pendingPlayerInfo;
    std::queue<int> pendingDisconnects;

    PacketManager() = default;
    PacketManager(PacketManager&) = delete;
    PacketManager& operator =(const PacketManager&) = delete;
    ~PacketManager() = default;

public:
    inline static PacketManager* Instance() {
        static PacketManager instance;
        return &instance;
    }

	// Flag para ocultar botones en la sala
    bool hideAllButtons = false;

    // Struct para almacenar información de ranking
    struct PlayerScore {
        std::string name;
        int score;
        int position;
    };

    // Variables públicas del juego
    std::vector<PlayerScore> ranking = std::vector<PlayerScore>();
    std::string myUsername;
	PlayerInfo playerInfo;
    int finalRanking[4] = { -1, -1, -1, -1 };
    int finishedCount = 0;
    std::string allUsernames[4];
    std::vector<int> disconnectedPlayers;

    bool ConnectToServer();
    void DisconnectFromServer();
    void Update();
    void Release();

    void HandShake(sf::Packet& data);
    void Login(sf::Packet& data);
    void Register(sf::Packet& data);

    void SendLoginRequest(const std::string& username, const std::string& password);
    void SendRegisterRequest(const std::string& username, const std::string& password);

    void CreateLobby(sf::Packet& data);
    void JoinLobby(sf::Packet& data);
    void OpenListenerHandler(sf::Packet& data);
    void PeerListHandler(sf::Packet& data);

    void GetRanking(sf::Packet& data);
    void RankingRequest();

    void CreateActionRequest(const std::string& idLobby);
    void JoinActionRequest(const std::string& idLobby);

    void SendToPeers(sf::Packet& packet);
    inline bool HasPendingAction() const { return !pendingActions.empty(); }
    std::pair<int, sf::Packet> PopPendingAction();

    void SendPlayerInfoToAll();
    void SendGreetingToAll();
    inline bool HasPendingPlayerInfo() const { return !pendingPlayerInfo.empty(); }
    std::pair<int, PlayerInfo> PopPendingPlayerInfo();

    inline int GetMyIndex() const { return myIndex; }
    inline int GetTotalPlayers() const { return totalPlayers; }
    inline bool IsP2PReady() const { return p2pReady; }
	inline bool IsPeerConnected(int playerID) const { return peerSockets.find(playerID) != peerSockets.end(); }
    inline bool HasFinished(int playerID) const {
        for (int i = 0; i < finishedCount; i++)
            if (finalRanking[i] == playerID) return true;
        return false;
    }
    inline bool IsDisconnected(int playerID) const {
        for (int id : disconnectedPlayers)
            if (id == playerID) return true;
        return false;
    }
    inline int GetFinishedCount() const { return finishedCount; }
    inline bool HasPendingDisconnect() const { return !pendingDisconnects.empty(); }
    int PopPendingDisconnect();

    void RecordWinner(int playerID);

	void SendTurnAction(int row, int col, int playerID);
    inline std::map<int, sf::TcpSocket*> GetPeerSockets() const { return peerSockets; }

    void DisconnectPeers();

    void SendWinNotificationToAll(int idPlayer);
    void SendGameResult();
};