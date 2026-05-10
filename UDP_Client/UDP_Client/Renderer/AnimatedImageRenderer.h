#pragma once
#include "ImageRenderer.h"
#include "../Managers/RenderManager.h"

class AnimatedImageRenderer : public ImageRenderer {
private:
    short frameWidth;
    short frameHeight;
    short fps;
    bool looping;
    float frameTime;
    float currentFrameTime;
    short rows;
    short columns;
    Vector2 currentFrame{ 1.0f, 1.0f };
public:
    AnimatedImageRenderer(short frameWidth, short frameHeight,
        short fps, bool looping, float frameTime,
        Transform* transform, std::string resourcePath, Vector2 sourceOffset, Vector2 sourceSize, short rows, short columns)
        : ImageRenderer(transform, resourcePath, sourceOffset, sourceSize),
        frameWidth(frameWidth), frameHeight(frameHeight), fps(fps), looping(looping), frameTime(frameTime), rows(rows), columns(columns) {
        this->color = color;
        currentFrameTime = 0.0f;
    }

    virtual void Update(float dt) override;
    virtual void Render() override;
};
