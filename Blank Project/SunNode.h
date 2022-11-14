#pragma once
#include "../nclgl/SceneNode.h"
#include "../nclgl/Vector3.h"

class SunNode : public SceneNode {
public:
	SunNode(OGLRenderer& renderer, Shader* combinePassShader, Mesh* quad);
	~SunNode();

	virtual void Update(float dt);
protected:
	Shader* combinePassShader = nullptr;
	SceneNode* sunLightNode = nullptr;

	float currentTime = 0.0f;

	const float TIME_SCALE = 1.0f;

	const float DAWN = 6.0f;
	const float NOON = 12.0f;
	const float DUSK = 18.0f;
	const float MIDN = 24.0f;

	const Vector3 AMBIENCE_DAWN = Vector3(0.2f, 0.2f, 0.1f);
	const Vector3 AMBIENCE_NOON = Vector3(0.3f, 0.2f, 0.1f);
	const Vector3 AMBIENCE_DUSK = Vector3(0.3f, 0.15f, 0.1f);
	const Vector3 AMBIENCE_MIDN = Vector3(0.1f, 0.1f, 0.2f);

	const Vector4 LIGHT_DAWN = Vector4(0.5f, 0.4f, 0.2f, 1.0f);
	const Vector4 LIGHT_NOON = Vector4(0.8f, 0.8f, 0.5f, 1.0f);
	const Vector4 LIGHT_DUSK = Vector4(0.4f, 0.2f, 0.2f, 1.0f);
	const Vector4 LIGHT_MIDN = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
};

