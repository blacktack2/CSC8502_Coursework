#pragma once
#include "../nclgl/SceneNode.h"
#include "../nclgl/Vector3.h"

#include <vector>

class SunNode : public SceneNode {
public:
	SunNode(OGLRenderer& renderer, Mesh* quad);
	~SunNode();

	virtual void Update(float dt);
protected:
	SceneNode* sunLightNode = nullptr;

	float currentTime = 0.0f;

	const float TIME_SCALE = 5.0f;

	const std::vector<std::pair<int, Vector3>> AMBIENCE = {
		{0.0f, Vector3(0.1f, 0.1f, 0.2f)},
		{6.0f, Vector3(0.1f, 0.1f, 0.2f)},
		{12.0f, Vector3(0.3f, 0.2f, 0.1f)},
		{18.0f, Vector3(0.1f, 0.1f, 0.2f)},
		{24.0f, Vector3(0.1f, 0.1f, 0.2f)}
	};
	const std::vector<std::pair<int, Vector3>> FOG = {
		{0.0f, Vector3(0.1f, 0.1f, 0.2f)},
		{6.0f, Vector3(0.1f, 0.1f, 0.2f)},
		{12.0f, Vector3(0.3f, 0.2f, 0.1f)},
		{18.0f, Vector3(0.1f, 0.1f, 0.2f)},
		{24.0f, Vector3(0.1f, 0.1f, 0.2f)}
	};
	const std::vector<std::pair<int, Vector4>> LIGHT = {
		{0.0f, Vector4(0.0f, 0.0f, 0.0f, 1.0f)},
		{6.0f, Vector4(0.0f, 0.0f, 0.0f, 1.0f)},
		{7.0f, Vector4(0.4f, 0.4f, 0.1f, 1.0f)},
		{12.0f, Vector4(0.8f, 0.8f, 0.5f, 1.0f)},
		{17.0f, Vector4(0.5f, 0.3f, 0.1f, 1.0f)},
		{18.0f, Vector4(0.0f, 0.0f, 0.0f, 1.0f)},
		{24.0f, Vector4(0.0f, 0.0f, 0.0f, 1.0f)}
	};
};

