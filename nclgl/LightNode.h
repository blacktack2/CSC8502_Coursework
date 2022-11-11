#pragma once
#include "SceneNode.h"

class LightNode : public SceneNode {
public:
	LightNode(const Vector4& position, const Vector4& colour, float radius,
		const Vector3 & direction, float angle);

	virtual void Update(float dt) override;
protected:
	Vector3 lightPos;
};

class PointLightNode : public LightNode {
public:
	PointLightNode(const Vector3& position = Vector3(), const Vector4& colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f), float radius = 0.0f);
};

class SpotLightNode : public LightNode {
public:
	SpotLightNode(const Vector3& position = Vector3(), const Vector4& colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f), float radius = 0.0f,
		const Vector3 & direction = Vector3(1.0f, 1.0f, 1.0f), float angle = 360.0f);
};

class DirectionLightNode : public LightNode {
public:
	DirectionLightNode(const Vector3& direction = Vector3(0.0f, 1.0f, 0.0f), const Vector4& colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
};
