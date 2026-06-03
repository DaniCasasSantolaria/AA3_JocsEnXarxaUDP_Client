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
    Vector2 currentFrame{ 0.0f, 0.0f };

    short currentAnimationRow = 0;
    short currentAnimationFrames = 0;
public:
    AnimatedImageRenderer(short frameWidth, short frameHeight,
        short fps, bool looping, float frameTime,
        Transform* transform, std::string resourcePath, Vector2 sourceOffset, Vector2 sourceSize, short rows, short columns)
        : ImageRenderer(transform, resourcePath, sourceOffset, sourceSize),
        frameWidth(frameWidth), frameHeight(frameHeight), fps(fps), looping(looping), frameTime(frameTime), rows(rows), columns(columns) {
        this->color = color;
        currentFrameTime = 0.0f;
    }
    void SetAnimation(short row, short totalFrames);
    virtual void Update(float dt) override;
};
