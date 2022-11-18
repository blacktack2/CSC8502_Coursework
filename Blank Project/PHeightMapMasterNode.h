#pragma once
#include "../nclgl/Camera.h"
#include "../nclgl/PHeightMapNode.h"
#include "../nclgl/SceneNode.h"

#include <unordered_map>

class PHeightMapMasterNode : public SceneNode {
public:
	PHeightMapMasterNode(OGLRenderer& renderer, Camera& camera);
	~PHeightMapMasterNode();

	virtual void Update(float dt) override;

	bool paused = false;
protected:
	void UpdateHeightMaps(int offsetX, int offsetZ, bool fromScratch = false);

	void CreateHeightMap(int x, int z);

	Mesh* quad;
	Mesh* heightMapQuad;
	Mesh* sphere;

	Camera& camera;

	float maxHeight = 100.0f;
	int heightMapResolution = 10;
	float heightMapWorldSize = 100.0f;

	int cameraOffsetX = 0;
	int cameraOffsetZ = 0;

	int generationRadius = 3;

	std::unordered_map<int, PHeightMapNode*> childHeightMaps{};
};

