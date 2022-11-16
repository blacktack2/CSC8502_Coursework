#include "PHeightMapMasterNode.h"

PHeightMapMasterNode::PHeightMapMasterNode(OGLRenderer& renderer, Camera& camera) : SceneNode(renderer), camera(camera) {
	Vector3 cameraPos = camera.GetPosition();
	cameraOffsetX = cameraPos.x / heightMapWorldSize;
	cameraOffsetZ = cameraPos.z / heightMapWorldSize;

	quad = Mesh::GenerateQuad();
	heightMapQuad = Mesh::GeneratePatchQuad(heightMapWorldSize);
	UpdateHeightMaps(0, 0, true);
}

PHeightMapMasterNode::~PHeightMapMasterNode() {
	delete quad;
	delete heightMapQuad;
}

void PHeightMapMasterNode::Update(float dt) {
	SceneNode::Update(dt);
	Vector3 cameraPos = camera.GetPosition();
	int offsetX = cameraPos.x / heightMapWorldSize;
	int offsetZ = cameraPos.z / heightMapWorldSize;
	if (offsetX != cameraOffsetX || offsetZ != cameraOffsetZ)
		UpdateHeightMaps(offsetX, offsetZ);
}

void PHeightMapMasterNode::UpdateHeightMaps(int offsetX, int offsetZ, bool fromScratch) {
	if (fromScratch) {
		for (int i = -generationRadius; i <= generationRadius; i++)
			for (int j = -generationRadius; j <= generationRadius; j++)
				CreateHeightMap(offsetX + i, offsetZ + j);
	} else {
		int deltaX = offsetX - cameraOffsetX;
		int deltaZ = offsetZ - cameraOffsetZ;
		for (auto iter = childHeightMaps.begin(); iter != childHeightMaps.end(); ) {
			int x = (iter->first & 255) - 128;
			int z = ((iter->first >> 8) & 255) - 128;
			if (x < offsetX - generationRadius || x > offsetX + generationRadius ||
				z < offsetZ - generationRadius || z > offsetZ + generationRadius) {
				RemoveChild(iter->second);
				iter = childHeightMaps.erase(iter);
			} else {
				iter++;
			}
		}
		for (int i = -generationRadius; i <= generationRadius; i++)
			for (int j = -generationRadius; j <= generationRadius; j++)
				if (childHeightMaps.find((offsetX + i + 128) | ((offsetZ + j + 128) << 8)) == childHeightMaps.end())
					CreateHeightMap(offsetX + i, offsetZ + j);
	}

	cameraOffsetX = offsetX;
	cameraOffsetZ = offsetZ;
}

void PHeightMapMasterNode::CreateHeightMap(int x, int z) {
	PHeightMapNode* heightMap = new PHeightMapNode(renderer, quad, heightMapQuad, x, z);
	AddChild(heightMap);
	childHeightMaps.emplace((x + 128) | ((z + 128) << 8), heightMap);
	heightMap->transform = Matrix4::Translation(Vector3(x * heightMapWorldSize, 0.0f, z * heightMapWorldSize)) * Matrix4::Rotation(-90, Vector3(1, 0, 0));
	heightMap->diffuseTex = renderer.GetTexture("earthTex");
	heightMap->bumpTex = renderer.GetTexture("earthBump");

	heightMap->maxHeight = maxHeight;
	heightMap->heightMapResolution = heightMapResolution;
	heightMap->heightMapWorldSize = heightMapWorldSize;
	heightMap->Generate();
}
