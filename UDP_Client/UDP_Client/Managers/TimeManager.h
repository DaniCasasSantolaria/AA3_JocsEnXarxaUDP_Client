#pragma once
#define TIME TimeManager::Instance()

#include <SFML/System.hpp>
#include <cmath>

class TimeManager {
private:
    TimeManager() {
        previousElapsedTime = clock.getElapsedTime().asSeconds();
        frameTime = 1.0f / static_cast<float>(FPS);
    }

    ~TimeManager() = default;
    TimeManager(const TimeManager& timeManager) = delete;
    TimeManager& operator=(const TimeManager& t) = delete;

    sf::Clock clock;

    float deltaTime = 0.0f;
    float elapsedTime = 0.0f;
    float previousElapsedTime = 0.0f;

    static const int FPS = 60;
    float frameTime = 0.0f;

public:
    inline float GetDeltaTime() const { return deltaTime; }
    inline float GetElapsedTime() const { return elapsedTime; }

    inline bool ShouldUpdateGame() const { return deltaTime >= frameTime; }

    inline void ResetDeltaTime() {
        deltaTime = deltaTime - (float)(floor(deltaTime / frameTime) * frameTime);
    }

    inline static TimeManager& Instance() {
        static TimeManager t;
        return t;
    }

    void Update();
};