#pragma once
#include <queue>
#include "Object.h"
#define SPAWN Spawner::Instance()

class Spawner {
private:
	std::queue<Object*> spawnedObjects;

	Spawner() = default;
	Spawner(Spawner& sp) = delete;
	Spawner operator=(Spawner* sp) = delete;
public:
	inline static Spawner& Instance() {
		static Spawner s;
		return s;
	}
	inline void SpawnObject(Object* obj) {
		spawnedObjects.push(obj);
	}
	inline Object* GetSpawnedObject() {
		if (spawnedObjects.empty())
			return nullptr;
		Object* o = spawnedObjects.front();
		spawnedObjects.pop();
		return o;
	}
	inline int GetSpawnedObjectsCount() {
		return spawnedObjects.size();
	}
	inline void ClearSpawnedObjects() {
		while (!spawnedObjects.empty()) {
			delete spawnedObjects.front();
			spawnedObjects.pop();
		}
	}
};