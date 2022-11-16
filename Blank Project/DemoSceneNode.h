#pragma once
#include "../nclgl/HeightMap.h"
#include "../nclgl/PHeightMapNode.h"
#include "../nclgl/SceneNode.h"

class DemoSceneNode : public SceneNode {
public:
	DemoSceneNode(OGLRenderer& renderer);
	~DemoSceneNode();

	virtual void Update(float dt);
protected:
	SceneNode* pointLightsNode = nullptr;
	SceneNode* spotLightsNode = nullptr;
	SceneNode* sunNode = nullptr;

	PHeightMapNode* heightMap = nullptr;
	Mesh* sphere = nullptr;
	Mesh* cube = nullptr;
	Mesh* quad = nullptr;
	Mesh* heightmapQuad = nullptr;

	GLuint earthTex = 0;
	GLuint earthBump = 0;

	GLuint blankTex = 0;
};

