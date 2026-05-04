#pragma once
#include "../ImageObject.h"

class Explosion : public ImageObject {
private:
	int radius;
	float timeOfExplosion = 2.0f;
	float timeStartExplosion = 0.0f;
	bool hasTakeInitialTime = false;
public:
	Explosion(std::string texturepath, Vector2 sourceOffset, Vector2 sourceSize, int numRows, int numColumns, int frameTime, bool hasToLoop, int radius)
		: ImageObject(texturepath, sourceOffset, sourceSize, numRows, numColumns, frameTime, hasToLoop), radius(radius) {}
	bool Explode();
};