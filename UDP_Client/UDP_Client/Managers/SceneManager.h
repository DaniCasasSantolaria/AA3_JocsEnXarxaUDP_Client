#pragma once
#include <unordered_map>
#include <string>
#include "../Scene.h"
#define SM SceneManager::Instance()

class SceneManager {
private:
	std::unordered_map<std::string, Scene*> scenes;
	Scene* currentScene = nullptr;
	std::string nextScene = "";
	/*bool isPlaying = true;*/

	SceneManager() = default;
	SceneManager(SceneManager& sm) = delete;
	SceneManager operator=(const SceneManager&) = delete;
public:
	inline static SceneManager& Instance() {
		static SceneManager sm;
		return sm;
	}
	inline bool AddScene(std::string name, Scene* scene) {
		if (scenes.find(name) == scenes.end()) {
			scenes.emplace(name, scene);
			return true;
		}
		return false;
	}
	inline Scene* GetScene(std::string name) {
		if (scenes.find(name) != scenes.end())
			return scenes[name];
		return nullptr;
	}
	inline bool SetCurrentScene(std::string name) {
		if (scenes.find(name) != scenes.end()) {
			currentScene = scenes[name];
			return true;
		}
		return false;
	}
	inline bool InitFirstScene(std::string name) {
		if (scenes.find(name) != scenes.end()) {
			currentScene = scenes[name];
			currentScene->OnEnter();
			return true;
		}
		return false;
	}
	inline Scene* GetCurrentScene() const { return currentScene; }
	inline bool SetNextScene(std::string name) {
		if (scenes.find(name) != scenes.end()) {
			nextScene = name;
			return true;
		}
		return false;
	}
	inline void UpdateCurrentScene() {
		if (nextScene != "") {
			currentScene->OnExit();
			currentScene = scenes[nextScene];
			currentScene->OnEnter();
			nextScene = "";
		}
		currentScene->Update();
	}
	/*inline void SetIsPlaying(bool isPlaying) { this->isPlaying = isPlaying; }
	inline bool GetIsPlaying() { return isPlaying; }*/
};