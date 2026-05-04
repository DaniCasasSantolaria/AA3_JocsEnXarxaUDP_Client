#pragma once

class Shooting {
protected:
	float shootCooldown;
	float lastTimeShooted = 0.0f;

public:
	Shooting(float shootCooldown)
		: shootCooldown(shootCooldown) { }
	virtual void Shoot() = 0;
};