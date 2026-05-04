#pragma once

class Movement {
protected:
	float speed = 0.0f;
public:
	virtual void Move() = 0;
};