#include "InputManager.h"
#include "TimeManager.h"
#include "../Managers/RenderManager.h"

InputManager::InputManager() {
    mouseX = 0;
    mouseY = 0;

    if (RM->GetWindow() != nullptr) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(*RM->GetWindow());
        mouseX = mousePos.x;
        mouseY = mousePos.y;
    }
}

InputManager& InputManager::Instance() {
    static InputManager instance;
    return instance;
}

bool InputManager::Listen() {
    lastTimeClick += TIME.GetDeltaTime();

    for (std::unordered_map<int, KeyState>::iterator it = keyReference.begin(); it != keyReference.end(); ++it) {
        if (it->second == DOWN) {
            it->second = HOLD;
        }
        else if (it->second == UP) {
            it->second = RELEASED;
        }
    }

    if (RM->GetWindow() != nullptr) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(*RM->GetWindow());
        mouseX = mousePos.x;
        mouseY = mousePos.y;
    }

    sf::RenderWindow* window = RM->GetWindow();
    if (window == nullptr) {
        return false;
    }

    while (const std::optional<sf::Event> event = window->pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            playing = false;
            return true;
        }
        else if (const sf::Event::MouseButtonPressed* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mousePressed->button == sf::Mouse::Button::Left) {
                leftClick = true;
                lastTimeClick = 0.0f;
            }
        }
        else if (const sf::Event::MouseButtonReleased* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (mouseReleased->button == sf::Mouse::Button::Left) {
                leftClick = false;
            }
        }
        else if (const sf::Event::KeyPressed* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            int keyCode = static_cast<int>(keyPressed->code);

            if (keyReference[keyCode] != HOLD) {
                keyReference[keyCode] = DOWN;
            }

            if (keyPressed->code == sf::Keyboard::Key::Backspace && textInputEnabled) {
                if (!inputText.empty()) {
                    inputText.pop_back();
                }

                if (inputText.empty()) {
                    inputText = "";
                }
            }
        }
        else if (const sf::Event::TextEntered* textEntered = event->getIf<sf::Event::TextEntered>()) {
            if (inputText.size() < 12 && textInputEnabled) {
                char entered = static_cast<char>(textEntered->unicode);

                if (textEntered->unicode >= 32 && textEntered->unicode < 127) {
                    if (inputText == " ") {
                        inputText = std::string(1, entered);
                    }
                    else {
                        inputText += entered;
                    }
                }
            }
        }
        else if (const sf::Event::KeyReleased* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
            int keyCode = static_cast<int>(keyReleased->code);

            if (keyReference[keyCode] != RELEASED) {
                keyReference[keyCode] = UP;
            }
        }
    }

    return false;
}