#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <map>
#include <vector>
#include <queue>
#include <utility>
#include <unordered_set>
#include "../Vector2.h"
#include "AudioManager.h"

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
    TAUNT,
    PING,
    PONG,
    DISCONNECTED_PLAYER,
    IRREGULARITY_WARNING,
    PLAYER_HEALTH_UPDATE,
    MATCH_FINISHED,
    SHOOT_ACK,
    SHOOT_CONFIRMED
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

enum matchResult {
    MATCH_RESULT_LOSE,
    MATCH_RESULT_WIN
};

enum matchFinishReason {
    FINISH_BY_LIVES,
    FINISH_BY_DISCONNECT,
    FINISH_BY_IRREGULARITY
};

// Informacion basica del jugador
struct PlayerInfo {
    std::string username = "";
    int score = 0;
};

#define NORMAL_PACKET 0b00000000
#define URGENT_PACKET 0b00000001
#define CRITIC_PACKET 0b00000010


#define MAX_PLAYERS 4

sf::Packet& operator <<(sf::Packet& packet, packetType type);
sf::Packet& operator <<(sf::Packet& packet, authResult result);
sf::Packet& operator <<(sf::Packet& packet, matchMode mode);
sf::Packet& operator <<(sf::Packet& packet, matchmakeStatus status);
sf::Packet& operator <<(sf::Packet& packet, movementPacketType status);
sf::Packet& operator <<(sf::Packet& packet, matchFinishReason reason);

sf::Packet& operator >>(sf::Packet& packet, packetType& type);
sf::Packet& operator >>(sf::Packet& packet, authResult& result);
sf::Packet& operator >>(sf::Packet& packet, matchMode& mode);
sf::Packet& operator >>(sf::Packet& packet, matchmakeStatus& status);
sf::Packet& operator >>(sf::Packet& packet, movementPacketType& status);
sf::Packet& operator >>(sf::Packet& packet, matchFinishReason& reason);

// Gestor de paquetes de red
// Responsable de manejar toda la comunicaci�n TCP/UDP del cliente
class PacketManager {
private:
    // Constantes de configuraci�n de red
    unsigned const short LISTENER_PORT = 55007; // Port
    const sf::IpAddress SERVER_IP = sf::IpAddress(10, 8, 0, 2); // IP

    // TCP Sockets de comunicaci�n
    sf::TcpSocket socket;

    //UDP
    unsigned const short UDP_SERVER_PORT = 55008;
    unsigned const short UDP_CLIENT_PORT = 55009;
    const sf::IpAddress UDP_SERVER_IP = sf::IpAddress(10, 8, 0, 2); // IP
    sf::UdpSocket udpSocket;
    bool udpConnected = false;


    // Estado de conexion
    unsigned short myIndex = 0;
    unsigned short totalPlayers = 0;
    bool serverConnected = false;

    bool matchFinishedReceived = false;
    matchResult lastMatchResult = MATCH_RESULT_LOSE;
    matchFinishReason lastMatchFinishReason = FINISH_BY_LIVES;

    //Ping Pong
    float lastUdpPacketTime = 0.0f;
    float lastPingTime = 0.0f;
    bool waitingPong = false;
    unsigned int lastPingId = 0;
    sf::Clock udpClock;

    //TAUNT
    unsigned short pendingTauntCount = 0;

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

    // Para actualizar vida y salud del enemigo
    struct EnemyHealthUpdate {
        unsigned short playerId = 0;
        short lives = 0;
        short health = 0;
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
    void HandleEnemyHealthUpdate(const char* buffer, std::size_t receivedSize, std::size_t readPos);


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


    //HIT
    struct HitConfirmedData {
        unsigned short targetPlayerId = 0;
    };

    void SendHit();
    void HandleHit(const char* buffer, std::size_t receivedSize, std::size_t readPos);
    inline bool HasPendingHitConfirmation() const { return !pendingHitConfirmations.empty(); }
    HitConfirmedData PopPendingHitConfirmation();

    //TAUNT
    void SendTaunt();
    void HandleTaunt(const char* buffer, std::size_t receivedSize, std::size_t readPos);
    inline bool HasPendingTaunt() const { return pendingTauntCount > 0; }
    inline void ConsumePendingTaunt() { if (pendingTauntCount > 0) pendingTauntCount--; }

    //SHOOT
    struct ShootData {
        unsigned short shooterNetworkId = 0;
        float directionX = 0.0f;
        float directionY = 0.0f;
        float spawnX = 0.0f;
        float spawnY = 0.0f;
    };

    void SendShoot(float spawnX, float spawnY, float directionX, float directionY);
    void SendShootAck(unsigned short criticalPacketId);
    void HandleShoot(const char* buffer, std::size_t receivedSize, std::size_t readPos);
    void HandleShootConfirmed(const char* buffer, std::size_t receivedSize, std::size_t readPos);
    inline bool HasPendingShoot() const { return !pendingShoots.empty(); }
    ShootData PopPendingShoot();
    inline bool HasShootConfirmed() const { return pendingShootConfirmedCount > 0; }
    inline void ConsumeShootConfirmed() { if (pendingShootConfirmedCount > 0) pendingShootConfirmedCount--; }

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

    //LIFE AND HEALTH UPDATE
    void SendLifeHealthUpdate(short lives, short health);

    inline bool HasPendingEnemyHealthUpdate() const {
        return !pendingEnemyHealthUpdates.empty();
    }

    EnemyHealthUpdate PopPendingEnemyHealthUpdate();

    //PING PONG
    void SendPing();
    void SendPong(unsigned int pingId);
    void HandlePing(const char* buffer, std::size_t receivedSize, std::size_t readPos);
    void HandlePong(const char* buffer, std::size_t receivedSize, std::size_t readPos);
    void UpdatePingSystem();
    void HandleDisconnectedPlayer(const char* buffer, std::size_t receivedSize, std::size_t readPos);
    void HandleIrregularityWarning(const char* buffer, std::size_t receivedSize, std::size_t readPos);
    void HandleMatchFinished(const char* buffer, std::size_t receivedSize, std::size_t readPos);

    //MATCH RESULT
    inline bool HasMatchFinished() const { return matchFinishedReceived; }
    inline matchResult GetLastMatchResult() const { return lastMatchResult; }
    inline matchFinishReason GetLastMatchFinishReason() const { return lastMatchFinishReason; }
    inline void ClearMatchFinished() { matchFinishedReceived = false; }

private:
    //Movement
    std::queue<OnlineMovement> pendingOnlineMovements;
    std::queue<LocalValidation> pendingLocalValidations;

    //Enemy Health Update
    std::queue<EnemyHealthUpdate> pendingEnemyHealthUpdates;

    //Shoot
    std::queue<ShootData> pendingShoots;
    std::unordered_set<unsigned short> processedShootIds;
    unsigned short pendingShootConfirmedCount = 0;

    //Hit
    std::queue<HitConfirmedData> pendingHitConfirmations;
};