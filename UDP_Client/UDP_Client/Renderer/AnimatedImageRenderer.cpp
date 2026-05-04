#include "AnimatedImageRenderer.h"

void AnimatedImageRenderer::Update(float dt) {
	ImageRenderer::Update(dt);
	if (currentFrameTime >= frameTime) {
		currentFrameTime = 0;

		if (currentFrame.x < columns && currentFrame.y >= rows) {
			sourceRect.position.y += frameHeight;
			sourceRect.position.x = 0;
			currentFrame.x++;
			currentFrame.y = 1;
		}
		else if (currentFrame.y < rows) {
			sourceRect.position.x += frameWidth;
			currentFrame.y++;
		}
		else if (looping && currentFrame.y == rows && currentFrame.x == columns) {
			sourceRect.position.x = 0;
			sourceRect.position.y = 0;
			currentFrame.y = 1;
			currentFrame.x = 1;
		}
	}
	else
		currentFrameTime += dt;
}

void AnimatedImageRenderer::Render() {
	//SDL_RenderCopy(RM->GetRenderer(), RM->GetTexture(targetPath), &sourceRect, &destRect);
}