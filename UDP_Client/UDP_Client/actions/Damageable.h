#pragma once

class Damageable {
public:
	virtual void RecieveDamage(short amount) = 0;
	virtual bool IsDead() = 0;
};