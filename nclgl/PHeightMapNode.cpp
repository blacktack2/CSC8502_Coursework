#include "PHeightMapNode.h"

PHeightMapNode::PHeightMapNode(OGLRenderer& renderer, Mesh* quad, Mesh* patchQuad, int x, int z) : SceneNode(renderer), heightMap(renderer, quad, x, z) {
	mesh = patchQuad;
	offsetX = x;
	offsetZ = z;

	shader = renderer.GetShader("heightmap");
	renderer.SetShaderUniformf("heightmap", "maxHeight", 100.0f);
}

PHeightMapNode::~PHeightMapNode() {
}

void PHeightMapNode::Generate() {
	modelScale = Vector3(heightMapWorldSize / 2.0f, heightMapWorldSize / 2.0f, 1.0f);
	heightMap.resolutionX = heightMapWorldSize;
	heightMap.resolutionZ = heightMapWorldSize;
	heightMap.worldX = heightMapResolution;
	heightMap.worldZ = heightMapResolution;
	heightMap.Generate();
}

void PHeightMapNode::DeGenerate() {
	heightMap.DeGenerate();
}

void PHeightMapNode::PreDrawMesh() {
	SceneNode::PreDrawMesh();
	renderer.SetShaderUniformf("heightmap", "tessLevelInner", heightMapResolution * heightMapWorldSize);
	renderer.SetShaderUniformf("heightmap", "tessLevelOuter", heightMapResolution * heightMapWorldSize);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, heightMap.GetHeightTex());
}
