#include "GridBox.h"
#include "../Managers/InputManager.h"

void GridBox::Update()
{
    ImageObject::Update();

    if (!Input.GetLeftClick()) {
        isLeftMouseDown = false;
    }
    if (physics->CheckOverlappingPoint(Vector2((float)Input.GetMouseX(), (float)Input.GetMouseY()))) {
        if (Input.GetLeftClick()) {
            if (!isLeftMouseDown) {
                DoAction();
                isLeftMouseDown = true;
            }
        }
    }
}
