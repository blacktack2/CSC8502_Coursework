#pragma once
#include "Vector3.h"
#include "Vector4.h"

struct Light {
	Light(const Vector3& p = Vector3(), const Vector4& c = Vector4(), float r = 0.0f) :
		position(p), radius(r), colour(c) {};

	Vector3 position;
	float radius;
	Vector4 colour;
};

