#pragma once
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/SceneNode.h"
#include "PHeightMapMasterNode.h"

class DemoSceneNode : public SceneNode {
public:
	DemoSceneNode(OGLRenderer& renderer, Camera& camera);
	~DemoSceneNode();

	virtual void Update(float dt);
protected:
	PHeightMapMasterNode* heightMapMaster = nullptr;
	SceneNode* pointLightsNode = nullptr;
	SceneNode* spotLightsNode = nullptr;
	SceneNode* sunNode = nullptr;

	Mesh* sphere = nullptr;
	Mesh* cube = nullptr;
	Mesh* quad = nullptr;
	Mesh* heightmapQuad = nullptr;

	int renderMode = 0;
};

