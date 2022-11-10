#pragma once
#include "SceneNode.h"

class PointLightNode : public SceneNode {
public:
	PointLightNode(const Vector3& position = Vector3(), const Vector4& colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f), float radius = 0.0f);
};

class SpotLightNode : public SceneNode {
public:
	SpotLightNode(const Vector3& position = Vector3(), const Vector4& colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f), float radius = 0.0f,
		const Vector3 & direction = Vector3(1.0f, 1.0f, 1.0f), float angle = 360.0f);
};

class DirectionLightNode : public SceneNode {
public:
	DirectionLightNode(const Vector3& direction = Vector3(0.0f, 1.0f, 0.0f), const Vector4& colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
};
