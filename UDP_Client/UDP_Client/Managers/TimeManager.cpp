#include "TimeManager.h"
#include <iostream>

void TimeManager::Update() {
    elapsedTime = clock.getElapsedTime().asSeconds();
    deltaTime += elapsedTime - previousElapsedTime;
    previousElapsedTime = elapsedTime;
}