#pragma once
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include "../Transform.h"
#include <string>

class Renderer {
protected:
    sf::Color color;
    Transform* transform;
    sf::IntRect sourceRect;
    sf::IntRect destRect;
    std::string targetPath;

public:
    Renderer(Transform* transform, std::string targetPath) {
        this->transform = transform;
        this->targetPath = targetPath;
    }

    virtual ~Renderer() {}

    virtual void Update(float dt) = 0;
    virtual void Render() = 0;

    virtual inline void SetColor(const sf::Color& color) { this->color = color; }
    inline sf::Color GetColor() const { return color; }
};