#pragma once
#include <SFML/Window.hpp>
#include <unordered_map>
#include <string>
#include "../TextObject.h"

#define Input InputManager::Instance()

enum KeyState { EMPTY, DOWN, UP, HOLD, RELEASED };

class InputManager {
private:
    int mouseX = 0;
    int mouseY = 0;
    float lastTimeClick = 0.4f;
    bool textInputEnabled = false;
    bool playing = true;
    bool leftClick = false;
    std::string inputText = "";

    std::unordered_map<int, KeyState> keyReference;

    InputManager();
    InputManager(const InputManager& inputManager) = delete;
    InputManager& operator=(const InputManager& inputManager) = delete;

public:
    static InputManager& Instance();

    bool Listen();

    inline int GetMouseX() const { return mouseX; }
    inline int GetMouseY() const { return mouseY; }
    inline bool GetLeftClick() const { return lastTimeClick <= 0.001f; }
    inline bool GetHoldClick() const { return leftClick; }
    inline bool GetEvent(int input, KeyState inputValue) { return keyReference[input] == inputValue; }
    inline void SetPlaying(bool isPlaying) { this->playing = isPlaying; }
    inline bool GetPlaying() const { return playing; }
    inline std::string GetInputText() const { return inputText; }
    inline void ClearInputText() { inputText.clear(); }
    inline void SetInputText(const std::string& text) { inputText = text; }
    inline void BeginTextInput() {
        textInputEnabled = true;
        inputText.clear();
    }
    inline void EndTextInput() { textInputEnabled = false; }
};