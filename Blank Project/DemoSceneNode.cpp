#include "DemoSceneNode.h"

#include "../nclgl/HeightMap.h"
#include "../nclgl/Light.h"

#include <algorithm>

DemoSceneNode::DemoSceneNode() {
	earthTex  = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG"    , SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthBump = SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (!earthTex || !earthBump)
		return;

	OGLRenderer::SetTextureRepeating(earthTex, true);
	OGLRenderer::SetTextureRepeating(earthBump, true);

	heightMap = new HeightMap(TEXTUREDIR"noise.png");
	sphere = Mesh::LoadFromMeshFile("Sphere.msh");
	cube = Mesh::LoadFromMeshFile("Cube.msh");

	Vector3 heightMapSize = heightMap->GetSize();

	SceneNode* heightMapNode = new SceneNode();
	AddChild(heightMapNode);
	heightMapNode->mesh = heightMap;
	heightMapNode->diffuseTex = earthTex;
	heightMapNode->bumpTex = earthBump;
	heightMapNode->boundingRadius = std::max(heightMapSize.x, heightMapSize.z);
	heightMapNode->transform = Matrix4::Translation(heightMapSize * -0.5f);

	pointLightsNode = new SceneNode();
	heightMapNode->AddChild(pointLightsNode);

	for (int i = 0; i < 10; i++) {
		SceneNode* lightNode = new SceneNode();
		pointLightsNode->AddChild(lightNode);
		lightNode->light = new Light(
			Vector4(rand() % (int)heightMapSize.x, 150.0f, rand() % (int)heightMapSize.z, 1.0f),
			Vector4(
				0.5f * (float)(rand() * (1.0f / (float)RAND_MAX)),
				0.5f * (float)(rand() * (1.0f / (float)RAND_MAX)),
				0.5f * (float)(rand() * (1.0f / (float)RAND_MAX)),
				1.0f
			),
			250.0f + (rand() % 500)
		);
		lightNode->lightMesh = sphere;
	}

	spotLightsNode = new SceneNode();
	heightMapNode->AddChild(spotLightsNode);

	for (int i = 0; i < 10; i++) {
		SceneNode* lightNode = new SceneNode();
		spotLightsNode->AddChild(lightNode);
		lightNode->light = new Light(
			Vector4(rand() % (int)heightMapSize.x, 150.0f, rand() % (int)heightMapSize.z, 1.0f),
			Vector4(
				0.5f * (float)(rand() * (1.0f / (float)RAND_MAX)),
				0.5f * (float)(rand() * (1.0f / (float)RAND_MAX)),
				0.5f * (float)(rand() * (1.0f / (float)RAND_MAX)),
				1.0f
			),
			1000.0f,
			Vector3(-0.5f + (rand() * (1.0f / (float)RAND_MAX)), 1.0f, -0.5f + (rand() * (1.0f / (float)RAND_MAX))),
			20.0f + (rand() % 80)
		);
		lightNode->lightMesh = sphere;
	}

	dirLightNode = new SceneNode();
	AddChild(dirLightNode);
	dirLightNode->light = new Light(
		Vector4(1.0f, 1.0f, 1.0f, 0.0f),
		Vector4(0.4f, 0.4f, 0.4f, 1.0f),
		10000.0f
	);
	dirLightNode->lightMesh = cube;
}

DemoSceneNode::~DemoSceneNode() {
	delete heightMap;
	delete sphere;
}

void DemoSceneNode::Update(float dt) {
	SceneNode::Update(dt);

	static float offset = 0.0f;
	offset += dt * 0.2;
	dirLightNode->light->position.x = std::cos(offset);
	dirLightNode->light->position.z = std::sin(offset);
}
