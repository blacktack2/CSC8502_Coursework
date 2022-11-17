#pragma once
#include "ProceduralHeightMap.h"
#include "SceneNode.h"

class PHeightMapNode : public SceneNode {
public:
	PHeightMapNode(OGLRenderer& renderer, Mesh* quad, Mesh* patchQuad, int x, int z);
	~PHeightMapNode();

	void Generate();
	void DeGenerate();

	ProceduralHeightMap heightMap;

	float maxHeight = 100.0f;
	int heightMapResolution = 10;
	float heightMapWorldSize = 100.0f;
protected:
	virtual void PreDrawMesh() override;
	virtual void PreDrawMeshDepth() override;

	int offsetX;
	int offsetZ;
};

