#include "DemoSceneNode.h"

#include "OrbiterNode.h"

#include "../nclgl/HeightMap.h"
#include "../nclgl/Light.h"
#include "../nclgl/LightNode.h"

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
	quad = Mesh::GenerateQuad();

	Vector3 heightMapSize = heightMap->GetSize();

	SceneNode* heightMapNode = new SceneNode();
	AddChild(heightMapNode);
	heightMapNode->mesh = heightMap;
	heightMapNode->diffuseTex = earthTex;
	heightMapNode->bumpTex = earthBump;
	heightMapNode->boundingRadius = std::max(heightMapSize.x, heightMapSize.z);
	heightMapNode->transform = Matrix4::Translation(heightMapSize * -0.5f);

	pointLightsNode = new SceneNode();
	AddChild(pointLightsNode);

	SceneNode* pointLightNode = new PointLightNode(
		Vector3(10.0f, 800.0f, 0.0f),
		Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		2000.0f
	);
	pointLightsNode->AddChild(pointLightNode);
	pointLightNode->lightMesh = sphere;

	spotLightsNode = new SceneNode();
	AddChild(spotLightsNode);

	SceneNode* spotLightNode = new SpotLightNode(
		Vector3(10.0f, 800.0f, 10.0f),
		Vector4(1.0f, 0.0f, 0.0f, 1.0f),
		2500.0f,
		Vector3(1.0f, -1.0f, 1.0f),
		40.0f
	);
	spotLightsNode->AddChild(spotLightNode);
	spotLightNode->lightMesh = sphere;

	dirLightNode = new DirectionLightNode(Vector3(0.0f, 1.0f, 0.0f), Vector4(0.4f, 0.4f, 0.4f, 1.0f));
	AddChild(dirLightNode);
	dirLightNode->lightMesh = quad;

	SceneNode* orbiterNodes = new SceneNode();
	heightMapNode->AddChild(orbiterNodes);
	orbiterNodes->transform = Matrix4::Translation(Vector3(0.0f, 300.0f, 0.0f));
	for (int i = 0; i < 10; i++) {
		SceneNode* orbitPoint = new SceneNode();
		orbiterNodes->AddChild(orbitPoint);
		orbitPoint->transform = Matrix4::Translation(Vector3(rand() % (int)heightMapSize.x, (float)(rand() % 200), rand() % (int)heightMapSize.z));
		OrbiterNode* orbiter = new OrbiterNode(10.0f + (rand() % 200), -10.0f + (rand() % 20));
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
}

void DemoSceneNode::Update(float dt) {
	SceneNode::Update(dt);

	static float offset = 0.0f;
	offset += dt;
	dirLightNode->light->direction.x = std::cos(offset);
	dirLightNode->light->direction.z = std::sin(offset);
}
