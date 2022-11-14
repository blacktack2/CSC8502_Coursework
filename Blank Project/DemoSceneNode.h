#pragma once
#include "..\nclgl\SceneNode.h"

class HeightMap;

class DemoSceneNode : public SceneNode {
public:
	DemoSceneNode(OGLRenderer& renderer);
	~DemoSceneNode();

	virtual void Update(float dt);
protected:
	SceneNode* pointLightsNode = nullptr;
	SceneNode* spotLightsNode = nullptr;
	SceneNode* sunNode = nullptr;

	HeightMap* heightMap = nullptr;
	Mesh* sphere = nullptr;
	Mesh* cube = nullptr;
	Mesh* quad = nullptr;

	GLuint earthTex = 0;
	GLuint earthBump = 0;
};

