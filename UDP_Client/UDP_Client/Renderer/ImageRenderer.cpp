#include "ImageRenderer.h"

void ImageRenderer::Update(float dt) {
	if (!transform) return;

	destRect.size.x = transform->size.x * transform->scale.x;
	destRect.size.y = transform->size.y * transform->scale.y;

	destRect.position.x = transform->position.x - destRect.size.x * 0.5f;
	destRect.position.y = transform->position.y - destRect.size.y * 0.5f;
}

void ImageRenderer::Render() {
	if (!RM || !transform) return;

	sf::RenderWindow* window = RM->GetWindow();
	if (!window) return;

	sf::Texture* tex = RM->GetTexture(targetPath);
	if (!tex) return;

	const sf::Vector2u texSize = tex->getSize();
	if (texSize.x == 0 || texSize.y == 0) return; // textura no cargada realmente

	sf::Sprite sprite (*tex);

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