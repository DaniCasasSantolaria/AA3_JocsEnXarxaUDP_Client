#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <map>
#include <vector>
#include <queue>
#include <utility>
#include "../Vector2.h"

#define PM PacketManager::Instance()

// Enum de tipos de paquetes que se pueden enviar/recibir
//TCP
enum packetType { 
    HANDSHAKE, 
    LOGIN, 
    REGISTER, 
    RANKING, 
    MATCHMAKE,
    WIN_NOTIFICATION, 
    GAME_RESULT,
    MAP_REQUEST };

//UDP
enum udpPacketType {
    MOVEMENT,
    REGISTER_CLIENT,
    SHOOT,
    HIT,
    PING,
    PONG,
    DISCONNECTED_PLAYER,
    IRREGULARITY_WARNING,
    MATCH_FINISHED
};

// Enum de resultados posibles en autenticaci�n
enum authResult { 
    LOGIN_OK, 
    USER_NOT_FOUND,
    WRONG_PASSWORD, 
    REGISTER_OK, 
    USER_ALREADY_EXISTS };

enum matchMode { 
    NON_COMPETITIVE, 
    COMPETITIVE };

enum matchmakeStatus { 
    QUEUE_WAITING, 
    MATCH_FOUND };

enum mapRequestType { 
    MAP_VERSION_CHECK, 
    MAP_UP_TO_DATE, 
    MAP_UPDATE };

enum movementPacketType {
    SEND_RAW_MOVEMENT,
    RECEIVE_VALIDATED_MOVEMENT
};

// Informaci�n b�sica del jugador
struct PlayerInfo {
    std::string username = "";
    int score = 0;
};

#define MAX_PLAYERS 4

sf::Packet& operator <<(sf::Packet& packet, packetType type);
sf::Packet& operator <<(sf::Packet& packet, authResult result);
sf::Packet& operator <<(sf::Packet& packet, matchMode mode);
sf::Packet& operator <<(sf::Packet& packet, matchmakeStatus status);
sf::Packet& operator <<(sf::Packet& packet, movementPacketType status);

sf::Packet& operator >>(sf::Packet& packet, packetType& type);
sf::Packet& operator >>(sf::Packet& packet, authResult& result);
sf::Packet& operator >>(sf::Packet& packet, matchMode& mode);
sf::Packet& operator >>(sf::Packet& packet, matchmakeStatus& status);
sf::Packet& operator >>(sf::Packet& packet, movementPacketType& status);

// Gestor de paquetes de red
// Responsable de manejar toda la comunicaci�n TCP/UDP del cliente
class PacketManager {
private:
    // Constantes de configuraci�n de red
    unsigned const short LISTENER_PORT = 55007; // Port
    const sf::IpAddress SERVER_IP = sf::IpAddress(10, 8, 0, 4); // IP

    // TCP Sockets de comunicaci�n
    sf::TcpSocket socket;

    //UDP
    unsigned const short UDP_SERVER_PORT = 55008;
    unsigned const short UDP_CLIENT_PORT = 55009;
    const sf::IpAddress UDP_SERVER_IP = sf::IpAddress(10, 8, 0, 2); // IP
    sf::UdpSocket udpSocket;
    bool udpConnected = false;
    unsigned int urgentBitmask = 00000001;
	unsigned int criticBitmask = 00000010;


    // Estado de conexi�n
    unsigned short myIndex = 0;
    unsigned short totalPlayers = 0;
    bool serverConnected = false;

    //Ping Pong
    float lastUdpPacketTime = 0.0f;
    float lastPingTime = 0.0f;
    bool waitingPong = false;
    unsigned int lastPingId = 0;
    sf::Clock udpClock;

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

    // Struct para almacenar informaci�n de ranking
    struct PlayerScore {
        std::string name;
        int score;
        unsigned short position;
    };

    //Movimiento online
    struct OnlineMovement {
        unsigned int movementId = 0;
        Vector2 position = Vector2(0.0f, 0.0f);
    };

    struct LocalValidation {
        unsigned int movementId = 0;
        Vector2 position = Vector2(0.0f, 0.0f);
    };

    // Variables p�blicas del juego
    std::vector<PlayerScore> ranking = std::vector<PlayerScore>();
    std::string myUsername;
    PlayerInfo playerInfo;
    short finalRanking[MAX_PLAYERS] = { -1, -1, -1, -1 };
    unsigned short finishedCount = 0;
    std::string allUsernames[MAX_PLAYERS];
    std::vector<unsigned short> disconnectedPlayers;

    inline unsigned short GetMyIndex() const { return myIndex; }

    bool ConnectToServer();
    void DisconnectFromServer();
    void Update();
    void Release();


    //RECIVIR PAQUETES
    void HandShake(sf::Packet& data);
    void Login(sf::Packet& data);
    void Register(sf::Packet& data);
	void Matchmake(sf::Packet& data);
    void HandleMapRequest(sf::Packet& packet);
    void HandleMovement(const char* buffer, std::size_t receivedSize, std::size_t readPos);


	//LOGIN Y REGISTER
    void SendLoginRequest(const std::string& username, const std::string& password);
    void SendRegisterRequest(const std::string& username, const std::string& password);


    //MATCHMAKE
    void SendMatchmakeRequest(matchMode mode);


    //MAP
    void RequestMap();
    unsigned short LoadLocalMapVersion();
    void SaveLocalMap(const std::string& mapContent);
    void SaveLocalMapVersion(unsigned short version);


    //RANKING
    void GetRanking(sf::Packet& data);
    void RankingRequest();


    //SHOOT
    struct ShootData {
        unsigned short shooterNetworkId = 0;
        float directionX = 0.0f;
        float directionY = 0.0f;
        float spawnX = 0.0f;
        float spawnY = 0.0f;
    };

    void SendShoot(float spawnX, float spawnY, float directionX, float directionY);
    void HandleShoot(const char* buffer, std::size_t receivedSize, std::size_t readPos);
    inline bool HasPendingShoot() const { return !pendingShoots.empty(); }
    ShootData PopPendingShoot();

    //MOVEMENT
	void SendMovement(float x, float y, unsigned int movementID);
    inline bool HasPendingOnlineMovement() const {
        return !pendingOnlineMovements.empty();
    }
    OnlineMovement PopPendingOnlineMovement();

    inline bool HasPendingLocalValidation() const {
        return !pendingLocalValidations.empty();
    }
    LocalValidation PopPendingLocalValidation();

    //PING PONG
    void SendPing();
    void SendPong(unsigned int pingId);
    void HandlePing(const char* buffer, std::size_t receivedSize, std::size_t readPos);
    void HandlePong(const char* buffer, std::size_t receivedSize, std::size_t readPos);
    void UpdatePingSystem();
    void HandleDisconnectedPlayer(const char* buffer, std::size_t receivedSize, std::size_t readPos);
    void HandleIrregularityWarning(const char* buffer, std::size_t receivedSize, std::size_t readPos);
    void HandleMatchFinished(const char* buffer, std::size_t receivedSize, std::size_t readPos);

private:
    //Movement
    std::queue<OnlineMovement> pendingOnlineMovements;
    std::queue<LocalValidation> pendingLocalValidations;

    //Shoot
    std::queue<ShootData> pendingShoots;
};