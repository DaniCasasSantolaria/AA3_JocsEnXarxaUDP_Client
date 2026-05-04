#pragma once

class Damageable {
public:
	virtual void RecieveDamage(int amount) = 0;
	virtual bool IsDead() = 0;
};