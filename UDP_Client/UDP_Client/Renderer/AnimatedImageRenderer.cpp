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

	if (!RM || !transform) return;

	sf::RenderWindow* window = RM->GetWindow();
	if (!window) return;

	sf::Texture* tex = RM->GetTexture(targetPath);
	if (!tex) return;

	const sf::Vector2u texSize = tex->getSize();
	if (texSize.x == 0 || texSize.y == 0) return; // textura no cargada realmente

	sf::Sprite sprite(*tex);

	// Recorte opcional
	if (sourceRect.size.x > 0.f && sourceRect.size.y > 0.f) {
		sf::IntRect texRect(
			{
				static_cast<int>(sourceRect.position.x),
				static_cast<int>(sourceRect.position.y)
			},
			{
				static_cast<int>(sourceRect.size.x),
				static_cast<int>(sourceRect.size.y)
			}
		);

		sprite.setTextureRect(texRect);
	}

	// Usar el tamaño real del sprite tras aplicar textureRect
	const auto bounds = sprite.getLocalBounds();
	if (bounds.size.x <= 0.f || bounds.size.y <= 0.f) return;

	sprite.setOrigin({
		bounds.size.x * 0.5f,
		bounds.size.y * 0.5f
		});

	// Como Update ya calcula el rect centrado, aquí lo más limpio es usar el centro real
	sprite.setPosition({
		transform->position.x,
		transform->position.y
		});

	sprite.setScale({
		destRect.size.x / bounds.size.x,
		destRect.size.y / bounds.size.y
		});

	sprite.setRotation(sf::degrees(transform->rotation));

	window->draw(sprite);
}