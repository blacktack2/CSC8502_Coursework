#pragma once
#include "Vector3.h"

struct Plane {
	Plane() = default;
	Plane(const Vector3& normal, float distance, bool normalise = false);
	~Plane() = default;

	inline bool SphereInside(const Vector3& position, float radius) const { return Vector3::Dot(position, normal) + distance > -radius; }

	Vector3 normal;
	float distance;
};

