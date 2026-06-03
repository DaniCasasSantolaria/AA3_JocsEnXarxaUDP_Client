#include "SplashScreen.h"
#include "Spawner.h"
#include "Managers/RenderManager.h"
#include "Managers/SceneManager.h"
#include "ImageObject.h"

void SplashScreen::OnEnter() {
	Object* o = new ImageObject("resources/SplashScreen.jpg", Vector2(0.0f, 0.0f), Vector2(1120.0f, 1120.0f));
	o->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f, 50.0f);
	o->GetTransform()->scale = Vector2(1.214f,1.214f);
	SPAWN.SpawnObject(o);
}

void SplashScreen::OnExit() {
	Scene::OnExit();
	SM.SetNextScene("Main Menu");
}

void SplashScreen::Update() {
	currentTime += TIME.GetDeltaTime();
	Scene::Update();
	for (int i = 0; i < objects.size(); i++) {
		objects[i]->GetRigidbody()->SetVelocity(Vector2(0.0f, 100.0f));
	}
	if (currentTime - timeToChangeScene > 0.0f)
		OnExit();
}

void SplashScreen::Render() {
	Scene::Render();
}