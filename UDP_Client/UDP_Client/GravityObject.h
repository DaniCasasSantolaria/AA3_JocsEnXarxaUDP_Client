#pragma once
#include "Object.h"
#include "Renderer/ImageRenderer.h"
#include "Renderer/AnimatedImageRenderer.h"

class GravityObject : public Object {
public:
	GravityObject(std::string texturepath, Vector2 sourceOffset, Vector2 sourceSize)
		: Object() {
		transform->size = sourceSize;
		physics->AddCollider(new AABB(sourceOffset, sourceSize * 0.2f));
		renderer = new ImageRenderer(transform, texturepath, sourceOffset, sourceSize);
		physics->SetGravity(98.0f);
	}
	GravityObject(std::string texturepath, Vector2 sourceOffset, Vector2 sourceSize, int numColumns, int numRows, float frameTime, bool hasToLoop)
		: Object() {
		transform->size = sourceSize;
		physics->AddCollider(new AABB(sourceOffset, sourceSize * 0.2f));
		renderer = new AnimatedImageRenderer(sourceSize.x, sourceSize.y, 60, hasToLoop, frameTime, transform, texturepath, sourceOffset, sourceSize, numRows, numColumns);
		physics->SetGravity(98.0f);
	}
};