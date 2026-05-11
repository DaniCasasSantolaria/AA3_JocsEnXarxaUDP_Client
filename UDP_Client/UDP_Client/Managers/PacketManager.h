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
enum packetType { HANDSHAKE, LOGIN, REGISTER, RANKING, MATCHMAKE, WIN_NOTIFICATION, GAME_RESULT, MAP_REQUEST, MAP_DATA };

// Enum de resultados posibles en autenticación
enum authResult { LOGIN_OK, USER_NOT_FOUND, WRONG_PASSWORD, REGISTER_OK, USER_ALREADY_EXISTS };

enum matchMode { NON_COMPETITIVE, COMPETITIVE };

// Información básica del jugador
struct PlayerInfo {
    std::string username;
    int score;
};

#define MAX_PLAYERS 4


sf::Packet& operator <<(sf::Packet& packet, packetType type);
sf::Packet& operator <<(sf::Packet& packet, authResult result);
sf::Packet& operator <<(sf::Packet& packet, matchMode mode);

sf::Packet& operator >>(sf::Packet& packet, packetType& type);
sf::Packet& operator >>(sf::Packet& packet, authResult& result);
sf::Packet& operator >>(sf::Packet& packet, matchMode& mode);
// Gestor de paquetes de red
// Responsable de manejar toda la comunicación TCP/IP del cliente
class PacketManager {
private:
    // Constantes de configuración de red
    unsigned const short LISTENER_PORT = 55007; // Port
    const sf::IpAddress SERVER_IP = sf::IpAddress(192, 168, 1, 131); // IP

    // Sockets de comunicación
    sf::TcpSocket socket;
    sf::TcpListener myListener;
    std::map<short, sf::TcpSocket*> peerSockets;
    std::vector<sf::TcpSocket*> pendingAccepts;

    // Estado de conexión
    short myIndex = -1;
    unsigned short totalPlayers = 0;
    bool serverConnected = false;
    bool p2pReady = false;
    std::string currentLobbyId;

    // Colas de eventos pendientes de procesar
    std::queue<std::pair<short, sf::Packet>> pendingActions;
    std::queue<std::pair<short, PlayerInfo>> pendingPlayerInfo;
    std::queue<short> pendingDisconnects;

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
        unsigned short position;
    };

    // Variables públicas del juego
    std::vector<PlayerScore> ranking = std::vector<PlayerScore>();
    std::string myUsername;
    PlayerInfo playerInfo;
    short finalRanking[MAX_PLAYERS] = { -1, -1, -1, -1 };
    unsigned short finishedCount = 0;
    std::string allUsernames[MAX_PLAYERS];
    std::vector<short> disconnectedPlayers;

    bool ConnectToServer();
    void DisconnectFromServer();
    void Update();
    void Release();

    void HandShake(sf::Packet& data);
    void Login(sf::Packet& data);
    void Register(sf::Packet& data);
	void Matchmake(sf::Packet& data);
	void MapData(sf::Packet& data);

    void SendLoginRequest(const std::string& username, const std::string& password);
    void SendRegisterRequest(const std::string& username, const std::string& password);

    void SendMatchmakeRequest(matchMode mode);

	void SendMapRequest();

    void PeerListHandler(sf::Packet& data);

    void GetRanking(sf::Packet& data);
    void RankingRequest();

    void SendToPeers(sf::Packet& packet);

    inline bool HasPendingAction() const { return !pendingActions.empty(); }
    std::pair<short, sf::Packet> PopPendingAction();

    std::pair<short, PlayerInfo> PopPendingPlayerInfo();

    inline short GetMyIndex() const { return myIndex; }
    inline unsigned short GetTotalPlayers() const { return totalPlayers; }
    inline bool IsP2PReady() const { return p2pReady; }
    inline bool IsPeerConnected(short playerID) const { return peerSockets.find(playerID) != peerSockets.end(); }
    inline bool HasFinished(short playerID) const {
        for (unsigned short i = 0; i < finishedCount; i++)
            if (finalRanking[i] == playerID) return true;
        return false;
    }
    inline bool IsDisconnected(short playerID) const {
        for (short id : disconnectedPlayers)
            if (id == playerID) return true;
        return false;
    }
    inline unsigned short GetFinishedCount() const { return finishedCount; }
    inline bool HasPendingDisconnect() const { return !pendingDisconnects.empty(); }
    short  PopPendingDisconnect();

    void RecordWinner(short playerID);

    inline std::map<short, sf::TcpSocket*> GetPeerSockets() const { return peerSockets; }

    void DisconnectPeers();

    void SendWinNotificationToAll(short idPlayer);
    void SendGameResult();
};