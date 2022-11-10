#pragma once
#include "Vector3.h"
#include "Vector4.h"

struct Light {
	Light(const Vector4& position = Vector4(), const Vector4& colour = Vector4(), float radius = 0.0f, const Vector3& direction = Vector3(), float angle = 360.0f) :
		position(position), radius(radius), colour(colour), direction(direction), angle(angle) {};

	Vector4 position;
	float radius;
	Vector4 colour;
	Vector3 direction;
	float angle;
};

