#include "Scene.h"
#include "Spawner.h"
#include "Managers/RenderManager.h"

void Scene::OnExit() {
	//Clear the spawner
	SPAWN.ClearSpawnedObjects();
	//Destroy ALL the objects
	for (Object* o : objects)
		delete o;
	objects.clear();
}

void Scene::Update() {
	//DESTRUCTION
	for (int i = objects.size() - 1; i >= 0; i--) {
		if (objects[i]->IsPendingDestroy()) {
			delete objects[i];
			objects.erase(objects.begin() + i);
		}
	}
	//SPAWNING
	while (SPAWN.GetSpawnedObjectsCount() > 0) {
		objects.push_back(SPAWN.GetSpawnedObject());
	}
	//UPDATE
	for (Object* o : objects)
		o->Update();

	//UPDATE PHYSICS
	for (int i = 0; i < objects.size(); i++) {
		for (int j = i + 1; j < objects.size(); j++) {
			if (objects[i]->GetRigidbody()->CheckCollision(objects[j]->GetRigidbody())) {
				objects[i]->OnCollisionEnter(objects[j]);
				objects[j]->OnCollisionEnter(objects[i]);
			}
		}
	}
}

void Scene::Render() {
	for (Object* o : objects)
		o->Render();
}
