#include "RenderManager.h"
#include <iostream>
#include <cassert>
#include <stdexcept>

void RenderManager::Init() {
    try {
        if (window == nullptr) {
            window = new sf::RenderWindow(
                sf::VideoMode(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT)),
                "Game",
                sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close
            );
        }

        if (!window->isOpen()) {
            throw std::runtime_error("No se pudo crear la ventana de SFML");
        }
    }
    catch (std::exception& exception) {
        std::cout << exception.what() << std::endl;
    }
}

void RenderManager::Release() {
    if (window != nullptr) {
        if (window->isOpen()) {
            window->close();
        }

        delete window;
        window = nullptr;
    }

    textures.clear();
    fonts.clear();
}

void RenderManager::ClearScreen() {
    if (window != nullptr) {
        window->clear(sf::Color::Black);
    }
}

void RenderManager::RenderScreen() {
    if (window != nullptr) {
        window->display();
    }
}

void RenderManager::LoadTexture(const std::string& path) {
    if (textures.find(path) != textures.end()) return;

    std::unique_ptr<sf::Texture> texture(new sf::Texture());

    if (!texture->loadFromFile(path)) {
        std::cout << "No se pudo cargar: " << path << std::endl;
        return;
    }

    textures[path] = std::move(texture);
}

sf::Texture* RenderManager::GetTexture(const std::string& path) {
    std::map<std::string, std::unique_ptr<sf::Texture>>::iterator it = textures.find(path);

    if (it != textures.end()) {
        return it->second.get();
    }

    return nullptr;
}

void RenderManager::LoadFont(const std::string& path) {
    if (fonts.find(path) != fonts.end()) {
        return;
    }

    std::unique_ptr<sf::Font> font(new sf::Font());

    bool loaded = font->openFromFile(path);
    //assert(loaded);

    fonts[path] = std::move(font);
}

sf::Font* RenderManager::GetFont(const std::string& path) {
    std::map<std::string, std::unique_ptr<sf::Font>>::iterator it = fonts.find(path);

    if (it != fonts.end()) {
        return it->second.get();
    }

    return nullptr;
}