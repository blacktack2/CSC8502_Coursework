#include "DemoSceneNode.h"

#include "../nclgl/Light.h"
#include "../nclgl/LightNode.h"
#include "OrbiterNode.h"
#include "SunNode.h"

#include <algorithm>

DemoSceneNode::DemoSceneNode(OGLRenderer& renderer) : SceneNode(renderer) {
	earthTex  = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG"    , SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthBump = SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	blankTex = SOIL_load_OGL_texture(TEXTUREDIR"blank.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (!earthTex || !earthBump)
		return;

	OGLRenderer::SetTextureRepeating(earthTex, true);
	OGLRenderer::SetTextureRepeating(earthBump, true);
	OGLRenderer::SetTextureRepeating(blankTex, true);

	float maxHeight = 100.0f;
	int heightMapResolution = 10;
	float heightMapWorldSize = 100.0f;

	sphere = Mesh::LoadFromMeshFile("Sphere.msh");
	cube = Mesh::LoadFromMeshFile("Cube.msh");
	quad = Mesh::GenerateQuad();
	heightmapQuad = Mesh::GeneratePatchQuad(heightMapWorldSize);

	for (int i = -5; i < 6; i++) {
		for (int j = -5; j < 6; j++) {
			PHeightMapNode* heightMap = new PHeightMapNode(renderer, quad, heightmapQuad, i, j);
			AddChild(heightMap);
			heightMap->transform = Matrix4::Translation(Vector3(i * heightMapWorldSize * 2, 0.0f, j * heightMapWorldSize * 2)) * Matrix4::Rotation(-90, Vector3(1, 0, 0));
			heightMap->diffuseTex = earthTex;
			heightMap->bumpTex = earthBump;

			heightMap->maxHeight = maxHeight;
			heightMap->heightMapResolution = heightMapResolution;
			heightMap->heightMapWorldSize = heightMapWorldSize;
			heightMap->Generate();
		}
	}

	pointLightsNode = new SceneNode(renderer);
	AddChild(pointLightsNode);

	SceneNode* pointLightNode = new PointLightNode(
		renderer,
		Vector3(10.0f, 100.0f, 0.0f),
		Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		1000.0f
	);
	pointLightsNode->AddChild(pointLightNode);
	pointLightNode->lightMesh = sphere;

	spotLightsNode = new SceneNode(renderer);
	AddChild(spotLightsNode);

	SceneNode* spotLightNode = new SpotLightNode(
		renderer,
		Vector3(0.0f, 500.0f, 150.0f),
		Vector4(1.0f, 0.0f, 0.0f, 1.0f),
		2500.0f,
		Vector3(1.0f, -1.0f, 1.0f),
		40.0f
	);
	spotLightsNode->AddChild(spotLightNode);
	spotLightNode->lightMesh = sphere;

	sunNode = new SunNode(renderer, quad);
	AddChild(sunNode);

	SceneNode* orbiterNodes = new SceneNode(renderer);
	AddChild(orbiterNodes);
	orbiterNodes->transform = Matrix4::Translation(Vector3(-2000.0f, 300.0f, -2000.0f));
	for (int i = 0; i < 10; i++) {
		SceneNode* orbitPoint = new SceneNode(renderer);
		orbiterNodes->AddChild(orbitPoint);
		orbitPoint->transform = Matrix4::Translation(Vector3(rand() % 4000, (float)(rand() % 200), rand() % 4000));
		OrbiterNode* orbiter = new OrbiterNode(renderer, 10.0f + (rand() % 200), -10.0f + (rand() % 20));
		orbitPoint->AddChild(orbiter);
		orbiter->modelScale = Vector3(1.0f, 1.0f, 1.0f) * (50.0f + (rand() % 150));
		orbiter->mesh = sphere;
		orbiter->diffuseTex = earthTex;
		orbiter->bumpTex = earthBump;
	}
}

DemoSceneNode::~DemoSceneNode() {
	delete heightMap;
	delete sphere;
	delete cube;
	delete quad;
	delete heightmapQuad;

	glDeleteTextures(1, &earthTex);
	glDeleteTextures(1, &earthBump);
	glDeleteTextures(1, &blankTex);
}

void DemoSceneNode::Update(float dt) {
	SceneNode::Update(dt);
	static float offset = 0.0;
	offset += dt;
	renderer.SetShaderUniformf("skybox", "cirrus", std::sin(offset) * 0.5 + 0.5);
	renderer.SetShaderUniformf("skybox", "cumulus", std::sin(offset * 0.5) * 0.5 + 0.5);
}
