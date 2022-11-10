#pragma once
#include "..\nclgl\SceneNode.h"

class HeightMap;

class DemoSceneNode : public SceneNode {
public:
	DemoSceneNode();
	~DemoSceneNode();

	virtual void Update(float dt);
protected:
	SceneNode* pointLightsNode = nullptr;
	SceneNode* spotLightsNode = nullptr;
	SceneNode* dirLightNode = nullptr;

	HeightMap* heightMap = nullptr;
	Mesh* sphere = nullptr;
	Mesh* cube = nullptr;

	GLuint earthTex = 0;
	GLuint earthBump = 0;
};

