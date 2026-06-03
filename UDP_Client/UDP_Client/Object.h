#pragma once
#include "Transform.h"
#include "Rigidbody.h"
#include "Renderer/Renderer.h"
#include "Managers/TimeManager.h"

class Object {
private:
	bool isPendingDestroy = false;
	
protected:
	Renderer* renderer = nullptr;
	Transform* transform;
	Rigidbody* physics;
public:
	Object() {
		transform = new Transform();
		physics = new Rigidbody(transform);
	}

	inline virtual void Update() {
		if(physics != nullptr)
			physics->Update(TIME.GetDeltaTime());
		renderer->Update(TIME.GetDeltaTime());
	}
	inline virtual void Render() { renderer->Render(); }
	inline Transform* GetTransform() const { return transform; }
	inline Rigidbody* GetRigidbody() const { return physics; }
	inline void SetColor(const sf::Color& c) { renderer->SetColor(c); }

	//De moment no fan res
	inline bool IsPendingDestroy() const { return isPendingDestroy; }
	inline virtual void Destroy() {
		isPendingDestroy = true;
	}
	virtual void OnCollisionEnter(Object* other) {}
	
	inline ~Object() {
		delete transform;
		delete physics;
		delete renderer;
	}
};