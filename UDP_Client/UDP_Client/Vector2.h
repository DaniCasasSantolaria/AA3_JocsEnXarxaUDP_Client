#pragma once
#include <corecrt_math.h>

class Vector2 {
public:
	float x;
	float y;
	Vector2()
		: x(0.0f), y(0.0f) {}
	Vector2(float _x, float _y) 
		: x(_x), y(_y) {}

	inline void Normalize() {
		float length = sqrt(x * x + y * y);
		x /= length;
		y /= length;
	}

	inline Vector2 const operator+(const Vector2 other) const { return Vector2(x + other.x, y + other.y); }
	inline void const operator+=(const Vector2 other) { x += other.x;  y += other.y; }
	inline Vector2 const operator-(const Vector2 other) const { return Vector2(x - other.x, y - other.y); }
	inline void const operator-=(const Vector2 other) { x -= other.x;  y -= other.y; }
	inline Vector2 const operator*(const float& other) const { return Vector2(x * other, y * other); }
	inline Vector2 const operator*(const Vector2 other) const { return Vector2(x * other.x, y * other.y); }
	inline Vector2 const operator/(const float& other) const { return Vector2(x / other, y / other); }
};