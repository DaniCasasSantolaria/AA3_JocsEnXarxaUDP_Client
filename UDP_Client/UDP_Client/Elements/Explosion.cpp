#include "Explosion.h"
#include "../Managers/TimeManager.h"

bool Explosion::Explode() {
	if (!hasTakeInitialTime) {
		timeStartExplosion = TIME.GetElapsedTime();
		hasTakeInitialTime = true;
	}

	if (TIME.GetElapsedTime() - timeStartExplosion < timeOfExplosion) {
		//Repeler los objetos
	}
	else {
		return false;	//Quan es fals s'ha de destruir el objecte
	}

	return true;
}