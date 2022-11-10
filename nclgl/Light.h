#pragma once
#include "Vector3.h"
#include "Vector4.h"

struct Light {
	Light(const Vector4& position = Vector4(0.0f, 0.0f, 0.0f, 1.0f), const Vector4& colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f), float radius = 0.0f,
		const Vector3& direction = Vector3(1.0f, 1.0f, 1.0f), float angle = 360.0f) :
		position(position), radius(radius), colour(colour), direction(direction.Normalised()), angle(angle) {};

	Vector4 position;
	float radius;
	Vector4 colour;
	Vector3 direction;
	float angle;
};

