#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <memory>

#define RM RenderManager::Instance()

class RenderManager {
private:
    sf::RenderWindow* window = nullptr;
    std::map<std::string, std::unique_ptr<sf::Texture>> textures;
    std::map<std::string, std::unique_ptr<sf::Font>> fonts;

    RenderManager() = default;
    RenderManager(RenderManager&) = delete;
    RenderManager& operator =(const RenderManager&) = delete;
    ~RenderManager() = default;

public:
    const unsigned int WINDOW_WIDTH = 1600;
    const unsigned int WINDOW_HEIGHT = 900;

    inline static RenderManager* Instance() {
        static RenderManager instance;
        return &instance;
    }

    void Init();
    void Release();
    void ClearScreen();
    void RenderScreen();
    sf::RenderWindow* GetWindow() const { return window; }

    void LoadTexture(const std::string& path);
    sf::Texture* GetTexture(const std::string& path);

    void LoadFont(const std::string& path);
    sf::Font* GetFont(const std::string& path);
};