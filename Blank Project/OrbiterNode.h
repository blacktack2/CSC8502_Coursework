#pragma once
#include "../nclgl/SceneNode.h"

class OrbiterNode : public SceneNode {
public:
	OrbiterNode(OGLRenderer& renderer, float radius = 1.0f, float speed = 1.0f);

	virtual void Update(float dt) override;
protected:
	float offset = 0.0f;

	float radius;
	float speed;
};

