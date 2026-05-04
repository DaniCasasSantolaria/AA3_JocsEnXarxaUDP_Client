#pragma once
#include "Object.h"
#include "Renderer/ImageRenderer.h"
#include "Renderer/AnimatedImageRenderer.h"

class ImageObject : public Object {
public:
	ImageObject(std::string texturepath, Vector2 sourceOffset, Vector2 sourceSize)
		: Object() {
		transform->size = sourceSize;
		physics->AddCollider(new AABB(sourceOffset, sourceSize * 0.2f));
		renderer = new ImageRenderer(transform, texturepath, sourceOffset, sourceSize);
	}
	ImageObject(std::string texturepath, Vector2 sourceOffset, Vector2 sourceSize, int numColumns, int numRows, float frameTime, bool hasToLoop)
		: Object() {
		transform->size = sourceSize;
		physics->AddCollider(new AABB(sourceOffset, sourceSize * 0.2f));
		renderer = new AnimatedImageRenderer(sourceSize.x, sourceSize.y, 60, hasToLoop, frameTime, transform, texturepath, sourceOffset, sourceSize, numRows, numColumns);
	}
};