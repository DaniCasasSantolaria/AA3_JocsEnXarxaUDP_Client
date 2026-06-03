#pragma once
#include "Renderer.h"
#include "../Managers/RenderManager.h"

class ImageRenderer : public Renderer {
public:
    ImageRenderer(Transform* transform, const std::string& resourcePath, Vector2 sourceOffset, Vector2 sourceSize)
        : Renderer(transform, resourcePath) {
        RM->LoadTexture(resourcePath);

        sourceRect = sf::IntRect(
            {
                static_cast<int>(sourceOffset.x),
                static_cast<int>(sourceOffset.y)
            },
            {
                static_cast<int>(sourceSize.x),
                static_cast<int>(sourceSize.y)
            }
        );

        destRect = sf::IntRect(
            {
                static_cast<int>(transform->position.x),
                static_cast<int>(transform->position.y)
            },
            {
                static_cast<int>(transform->scale.x),
                static_cast<int>(transform->scale.y)
            }
        );
    }

    virtual void Update(float dt) override;
    virtual void Render() override;
};