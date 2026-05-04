#pragma once
#include "../Scene.h"
#include "../MenuElements/Button.h"

// Estructura para almacenar información de puntuación de un jugador
struct PlayerScore {
    std::string name;
    int score;
    int position;
};

class RankingScreen : public Scene {
private:
    // Lista de puntuaciones de todos los jugadores
    std::vector<PlayerScore> scores;

    // Método para cargar las puntuaciones desde PacketManager
    void LoadScore();
public:
    RankingScreen() = default;
    void OnEnter() override;
    void OnExit() override;
    void Update() override;
};