#pragma once
#include "Vector2.h"

class AABB {	//Axis Align Bounding Box
protected:
	Vector2 topLeft;
	Vector2 size;
public:
	AABB(Vector2 _topLeft, Vector2 _size)
		: topLeft(_topLeft), size(_size) {}
	inline Vector2 GetTopLeft() { return topLeft; }
	inline Vector2 GetSize() { return size; }
	inline void SetTopLeft(Vector2 newTopLeft) { topLeft = newTopLeft; }
	inline void SetSize(Vector2 newSize) { size = newSize; }

	bool CheckOverlappingPoint(Vector2 point);
	bool CheckOverlappingAABB(const AABB* other);
};