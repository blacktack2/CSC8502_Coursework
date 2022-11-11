#include "LightNode.h"

#include "Light.h"

LightNode::LightNode(const Vector4& position, const Vector4& colour, float radius, const Vector3& direction, float angle) {
	light = new Light(position, colour, radius, direction, angle);
	lightPos = Vector3(position.x, position.y, position.z);
}

void LightNode::Update(float dt) {
	SceneNode::Update(dt);
	Vector3 position = light->position.w == 0.0f ? Vector3() : worldTransform * lightPos;
	light->position = Vector4(position.x, position.y, position.z, light->position.w);
}

PointLightNode::PointLightNode(const Vector3& position, const Vector4& colour, float radius) :
	LightNode(Vector4(position.x, position.y, position.z, 1.0f), colour, radius, Vector3(), 360.0f) {
}

SpotLightNode::SpotLightNode(const Vector3& position, const Vector4& colour, float radius, const Vector3& direction, float angle) :
	LightNode(Vector4(position.x, position.y, position.z, 1.0f), colour, radius, direction, angle) {
}

DirectionLightNode::DirectionLightNode(const Vector3& direction, const Vector4& colour) :
	LightNode(Vector4(0.0f, 0.0f, 0.0f, 0.0f), colour, 0.0f, Vector3(direction.x, direction.y, direction.z).Normalised(), 0.0f) {
}
