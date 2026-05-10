#include "AnimatedImageRenderer.h"

void AnimatedImageRenderer::SetAnimation(short row, short totalFrames) {
	if (currentAnimationRow == row && currentAnimationFrames == totalFrames) {
        return;
    }

    currentAnimationRow = row;
    currentAnimationFrames = totalFrames;

    currentFrame.x = 0;
    currentFrame.y = row;

    currentFrameTime = 0.0f;

    sourceRect.position.x = 0;
    sourceRect.position.y = row * frameHeight;
}

void AnimatedImageRenderer::Update(float dt) {
    ImageRenderer::Update(dt);

    currentFrameTime += dt;

    if (currentFrameTime >= frameTime) {
        currentFrameTime = 0.0f;

        currentFrame.x++;

        if (currentFrame.x >= currentAnimationFrames) {
            if (looping) {
                currentFrame.x = 0;
            }
            else {
                currentFrame.x = currentAnimationFrames - 1;
            }
        }

        sourceRect.position.x = currentFrame.x * frameWidth;
        sourceRect.position.y = currentAnimationRow * frameHeight;
    }
}