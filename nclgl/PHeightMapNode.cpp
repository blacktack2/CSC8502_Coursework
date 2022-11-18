#include "PHeightMapNode.h"
#include "../nclgl/LightNode.h"

PHeightMapNode::PHeightMapNode(OGLRenderer& renderer, Mesh* quad, Mesh* patchQuad, Mesh* sphere, int x, int z) : SceneNode(renderer), heightMap(renderer, quad, x, z), sphere(sphere) {
	mesh = patchQuad;
	offsetX = x;
	offsetZ = z;

	shader = renderer.GetShader("heightmap");
	depthShader = renderer.GetShader("heightmapDepth");
}

PHeightMapNode::~PHeightMapNode() {
}

void PHeightMapNode::Generate() {
	renderer.SetShaderUniformf("heightmap", "maxHeight", maxHeight);
	renderer.SetShaderUniformf("heightmapDepth", "maxHeight", maxHeight);

	modelScale = Vector3(heightMapWorldSize / 2.0f, 1.0f, heightMapWorldSize / 2.0f);

	heightMap.resolutionX = heightMapWorldSize;
	heightMap.resolutionZ = heightMapWorldSize;
	heightMap.worldX = heightMapResolution;
	heightMap.worldZ = heightMapResolution;
	heightMap.Generate();

	if (offsetX % 4 == 0 && offsetZ % 4 == 0) {
		SceneNode* pointLightNode = new PointLightNode(
			renderer,
			Vector3(0.0f, 100.0f, 0.0f),
			Vector4(0.5f + 0.5f * (float)rand() / (float)RAND_MAX, 0.5f + 0.5f * (float)rand() / (float)RAND_MAX, 0.5f + 0.5f * (float)rand() / (float)RAND_MAX, 1.0f),
			80.0f
		);
		AddChild(pointLightNode);
		pointLightNode->lightMesh = sphere;
	} else if (offsetX % 4 == 2 && offsetZ % 4 == 2) {
		SceneNode* spotLightNode = new SpotLightNode(
			renderer,
			Vector3(10.0f, 100.0f, 0.0f),
			Vector4(0.7f + 0.3f * (float)rand() / (float)RAND_MAX, 0.7f + 0.3f * (float)rand() / (float)RAND_MAX, 0.7f + 0.3f * (float)rand() / (float)RAND_MAX, 1.0f),
			500.0f,
			Vector3(0.2f - 0.4 * (float)rand() / (float)RAND_MAX, 0.2f - 0.4 * (float)rand() / (float)RAND_MAX, 1.0f),
			5.0f + 15.0f * (float)rand() / (float)RAND_MAX
		);
		AddChild(spotLightNode);
		spotLightNode->lightMesh = sphere;
	}
}

void PHeightMapNode::DeGenerate() {
	heightMap.DeGenerate();
	ClearChildren();
}

void PHeightMapNode::PreDrawMesh() {
	SceneNode::PreDrawMesh();
	renderer.SetShaderUniformf("heightmap", "tessLevelInner", heightMapResolution * heightMapWorldSize);
	renderer.SetShaderUniformf("heightmap", "tessLevelOuter", heightMapResolution * heightMapWorldSize);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, heightMap.GetHeightTex());
}

void PHeightMapNode::PreDrawMeshDepth() {
	SceneNode::PreDrawMeshDepth();
	renderer.SetShaderUniformf("heightmapDepth", "tessLevelInner", heightMapResolution * heightMapWorldSize);
	renderer.SetShaderUniformf("heightmapDepth", "tessLevelOuter", heightMapResolution * heightMapWorldSize);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, heightMap.GetHeightTex());
}
