#include "Plane.h"

Plane::Plane(const Vector3& n, float d, bool normalise) {
	float length = normalise ? 1.0f / std::sqrt(Vector3::Dot(n, n)) : 1.0f;

	distance = d * length;
	normal = n * length;
}
