#pragma once
#include <SFML/Graphics.hpp>
#include "Renderer.h"
#include "Managers/RenderManager.h"
#include <string>
#include <memory>

#define FONT_PATH "resources/fonts/ScienceGothic-Regular.ttf"

class TextRenderer : public Renderer {
protected:
    std::string text;
    sf::Text* textDrawable;
    bool autoSize = true;

public:
    TextRenderer(Transform* transform, const std::string& text)
        : Renderer(transform, FONT_PATH) {
        SetText(text);
    }

    ~TextRenderer() override = default;

    void SetText(const std::string& newText) {
        text = newText;

        RM->LoadFont(targetPath);

        sf::Font* font = RM->GetFont(targetPath);
        if (font == nullptr) {
            return;
        }

        if (textDrawable == nullptr) {
            textDrawable = (new sf::Text(*font, text, 24)
            );
        }
        else {
            textDrawable->setFont(*font);
            textDrawable->setString(text);
            textDrawable->setCharacterSize(24);
        }

        textDrawable->setFillColor(color);

        sf::FloatRect bounds = textDrawable->getLocalBounds();

        sourceRect.position.x = 0;
        sourceRect.position.y = 0;
        sourceRect.size.x = static_cast<int>(bounds.size.x);
        sourceRect.size.y = static_cast<int>(bounds.size.y);
    }

    inline void Update(float dt) override {
        (void)dt;

        if (textDrawable == nullptr) {
            return;
        }

        sf::FloatRect bounds = textDrawable->getLocalBounds();

        if (autoSize) {
            destRect.size.x = static_cast<int>(bounds.size.x * transform->scale.x);
            destRect.size.y = static_cast<int>(bounds.size.y * transform->scale.y);
        }
        else {
            destRect.size.x = static_cast<int>(transform->size.x * transform->scale.x);
            destRect.size.y = static_cast<int>(transform->size.y * transform->scale.y);
        }

        Vector2 centerOffset = Vector2(
            static_cast<float>(destRect.size.x),
            static_cast<float>(destRect.size.y)
        ) / 2.0f;

        destRect.position.x = static_cast<int>(transform->position.x - centerOffset.x);
        destRect.position.y = static_cast<int>(transform->position.y - centerOffset.y);

        textDrawable->setFillColor(color);
        textDrawable->setRotation(sf::degrees(transform->rotation));

        float scaleX = 1.0f;
        float scaleY = 1.0f;

        if (bounds.size.x > 0.0f) {
            scaleX = static_cast<float>(destRect.size.x) / bounds.size.x;
        }

        if (bounds.size.y > 0.0f) {
            scaleY = static_cast<float>(destRect.size.y) / bounds.size.y;
        }

        textDrawable->setOrigin(
            sf::Vector2f(
                bounds.position.x + bounds.size.x / 2.0f,
                bounds.position.y + bounds.size.y / 2.0f
            )
        );

        textDrawable->setPosition(
            sf::Vector2f(transform->position.x, transform->position.y)
        );

        textDrawable->setScale(sf::Vector2f(scaleX, scaleY));
    }

    inline void Render() override {
        if (textDrawable != nullptr) {
            RM->GetWindow()->draw(*textDrawable);
        }
    }

    inline void SetColor(const sf::Color& newColor) override {
        Renderer::SetColor(newColor);

        if (textDrawable != nullptr) {
            textDrawable->setFillColor(color);
        }
    }
};