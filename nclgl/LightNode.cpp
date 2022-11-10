#include "LightNode.h"

#include "Light.h"

PointLightNode::PointLightNode(const Vector3& position, const Vector4& colour, float radius) {
	light = new Light(Vector4(position.x, position.y, position.z, 1.0f), colour, radius);
}

SpotLightNode::SpotLightNode(const Vector3& position, const Vector4& colour, float radius, const Vector3& direction, float angle) {
	light = new Light(Vector4(position.x, position.y, position.z, 1.0f), colour, radius, direction, angle);
}

DirectionLightNode::DirectionLightNode(const Vector3& direction, const Vector4& colour) {
	light = new Light(Vector4(direction.x, direction.y, direction.z, 0.0f).Normalised(), colour, 10000.0f);
}
