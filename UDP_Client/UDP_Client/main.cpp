#include <SFML/Network.hpp>
#include <iostream>
#include <cstring>

/*#define SERVER_PORT 55008
const sf::IpAddress SERVER_IP = sf::IpAddress(127, 0, 0, 1);

void main() {
	sf::UdpSocket serverSocket;

	char buffer[1024];
	std::size_t bufferDataSize = 0;
	int data = 0;
	std::string message = "hola";
	std::string final = message + std::to_string(data);
	for (int i = 0; i < 100000; ++i) {
		int messageSize = message.size();
		std::memcpy(buffer + bufferDataSize, &messageSize, sizeof(messageSize));
		bufferDataSize += (sizeof(messageSize));

		std::memcpy(buffer + bufferDataSize, message.c_str(), messageSize);
		bufferDataSize += messageSize;

		std::memcpy(buffer + bufferDataSize, &i, sizeof(i));
		bufferDataSize += (sizeof(i));

		if (serverSocket.send(buffer, bufferDataSize, SERVER_IP, SERVER_PORT) == sf::Socket::Status::Done) {
			std::cout << "Mensaje entregado al sistema operativo" << std::endl;
			data += 1;
			//final = message + std::to_string(data);
			bufferDataSize = 0;
			//return;
		}
	}
}*/

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include "Game.h"
#include "Managers/InputManager.h"

#define WIDTH 1280
#define HEIGHT 720


#define FRAME_WIDTH 96
#define FRAME_HEIGHT 101
#define NUM_FRAMES 10
#define ANIMATION_SPEED 0.1f

void Render(sf::RenderWindow& window, sf::RectangleShape& square /*sf::Sprite& animatedSprite*/) {
	window.clear(sf::Color(0x000000FF));
	window.draw(square);
	//window.draw(animatedSprite);
	window.display();
}

sf::Texture LoadSpriteSheet(const std::string& filePath) {
	sf::Texture texture;

	if (!texture.loadFromFile(filePath)) {
		std::cout << "Error loading sprite sheet" << filePath << std::endl;
		return texture;
	}
	return texture;
}

void HandleEvent(const sf::Event& event, sf::RenderWindow& window) {
	if (event.is<sf::Event::Closed>()) {
		window.close();
	}

	if (const sf::Event::KeyPressed* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
		switch (keyPressed->code) {
		case sf::Keyboard::Key::Escape:
			window.close();
			break;
		default:
			break;
		}
	}

	if (const sf::Event::MouseButtonPressed* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
		switch (mousePressed->button) {
		case sf::Mouse::Button::Left:
			std::cout << "Left mouse button pressed at (" << mousePressed->position.x << ", " << mousePressed->position.y << ")\n";
			break;
		default:
			break;
		}
	}
}

void UpdateAnimation(sf::Sprite& animatedSprite, int& currentFrame, float& deltaTimeAnimation) {
	if (deltaTimeAnimation >= ANIMATION_SPEED) {
		currentFrame = (currentFrame + 1) % NUM_FRAMES;
		animatedSprite.setTextureRect(sf::IntRect({ currentFrame * FRAME_WIDTH, 0 }, { FRAME_WIDTH, FRAME_HEIGHT }));
		deltaTimeAnimation = 0.0f;
	}
}

int main() {

	srand(time(NULL));
	Game game;

	try {
		game.Init();
	}
	catch (std::exception& exception) {
		std::cout << "Error: " << exception.what() << std::endl;
		game.Release();
		return -1;
	}
	bool playing = true;
	while (playing) {
		TIME.Update();
		if (TIME.ShouldUpdateGame()) {
			//std::cout << TIME.GetElapsedTime() << std::endl;
			playing = !Input.Listen();
			if (!Input.GetPlaying())
				playing = false;
			else
				playing = true;
			game.Update();
			game.Render();
			TIME.ResetDeltaTime();
		}
	}

	game.Release();

	return 0;
}