#pragma once
#include "ImageRenderer.h"
#include "../Managers/RenderManager.h"

class AnimatedImageRenderer : public ImageRenderer {
private:
    int frameWidth;
    int frameHeight;
    int fps;
    bool looping;
    float frameTime;
    float currentFrameTime;
    int rows;
    int columns;
    Vector2 currentFrame{ 1.0f, 1.0f };
public:
    AnimatedImageRenderer(int frameWidth, int frameHeight,
        int fps, bool looping, float frameTime,
        Transform* transform, std::string resourcePath, Vector2 sourceOffset, Vector2 sourceSize, int rows, int columns)
        : ImageRenderer(transform, resourcePath, sourceOffset, sourceSize),
        frameWidth(frameWidth), frameHeight(frameHeight), fps(fps), looping(looping), frameTime(frameTime), rows(rows), columns(columns) {
        this->color = color;
        currentFrameTime = 0.0f;
    }

    virtual void Update(float dt) override;
    virtual void Render() override;
};
