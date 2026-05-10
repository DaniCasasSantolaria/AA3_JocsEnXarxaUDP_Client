#pragma once
#include "../ImageObject.h"
#include "../Renderer/ImageRenderer.h"

#include <string>
#include <functional>

class GridBox : public ImageObject {
protected:
    bool isLeftMouseDown = false;
public:
    bool isEmpty = true;
    short playerID = -1;
    std::function<void()> action;   //Fa la mateixa funció que el butó

    GridBox(const std::string& path, Vector2 sourceOffset, Vector2 sourceSize)
        : ImageObject(path, sourceOffset, sourceSize) {}

    void ApplyImage() {
        std::string path;

        if (!isEmpty) {
            if (playerID == 0)
                path = "resources/Gameplay/GridCircle.png";
            else if (playerID == 1)
                path = "resources/Gameplay/GridTriangle.png";
			else if (playerID == 2)
				path = "resources/Gameplay/GridSquare.png";
			else if (playerID == 3)
				path = "resources/Gameplay/GridCross.png";
        }
        else {
			return;
        }

        delete renderer;
        renderer = new ImageRenderer(transform, path, Vector2{ 0, 0 }, transform->size);
    }

    inline void SetAction(std::function<void()> a) { action = a; }
    inline void DoAction() { if (action) action(); }

    virtual void Update() override;
};