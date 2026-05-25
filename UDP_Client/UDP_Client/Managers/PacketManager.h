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
enum packetType { HANDSHAKE, LOGIN, REGISTER, RANKING, MATCHMAKE, WIN_NOTIFICATION, GAME_RESULT, MAP_REQUEST };
enum udpPacketType { MOVEMENT };

// Enum de resultados posibles en autenticación
enum authResult { LOGIN_OK, USER_NOT_FOUND, WRONG_PASSWORD, REGISTER_OK, USER_ALREADY_EXISTS };

enum matchMode { NON_COMPETITIVE, COMPETITIVE };

enum matchmakeStatus { QUEUE_WAITING, MATCH_FOUND };

enum mapRequestType { MAP_VERSION_CHECK, MAP_UP_TO_DATE, MAP_UPDATE };

enum movementPacketType { SEND_RAW_MOVEMENT, RECEIVE_VALIDATED_MOVEMENT };

// Información básica del jugador
struct PlayerInfo {
    std::string username;
    int score;
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
// Responsable de manejar toda la comunicación TCP/IP del cliente
class PacketManager {
private:
    // Constantes de configuración de red
    unsigned const short LISTENER_PORT = 55007; // Port
    const sf::IpAddress SERVER_IP = sf::IpAddress(10, 8, 0, 3); // IP

    // TCP Sockets de comunicación
    sf::TcpSocket socket;
    sf::TcpListener myListener;
    std::map<short, sf::TcpSocket*> peerSockets;
    std::vector<sf::TcpSocket*> pendingAccepts;

    //UDP
    unsigned const short UDP_SERVER_PORT = 55008;
    const sf::IpAddress UDP_SERVER_IP = sf::IpAddress(10, 8, 0, 3); // IP
    sf::UdpSocket udpSocket;
    bool udpConnected = false;
    unsigned int urgentBitmask = 00000001;
	unsigned int criticBitmask = 00000010;


    // Estado de conexión
    short myIndex = -1;
    unsigned short totalPlayers = 0;
    bool serverConnected = false;
    bool p2pReady = false;
    std::string currentLobbyId;

    // Colas de eventos pendientes de procesar
    std::queue<std::pair<short, sf::Packet>> pendingActions;
    std::queue<std::pair<short, PlayerInfo>> pendingPlayerInfo;

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


    //MOVEMENT
	void SendMovement(float x, float y, unsigned int movementID);
};