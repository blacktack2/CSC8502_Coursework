#include "DemoSceneNode.h"

#include "../nclgl/Light.h"
#include "../nclgl/LightNode.h"
#include "OrbiterNode.h"
#include "SunNode.h"

#include <algorithm>

DemoSceneNode::DemoSceneNode(OGLRenderer& renderer, Camera& camera) : SceneNode(renderer) {
	float maxHeight = 100.0f;
	int heightMapResolution = 10;
	float heightMapWorldSize = 100.0f;

	camera.SetPosition(Vector3(0.0f, heightMapWorldSize, 0.0f));

	GLuint earthTex = renderer.GetTexture("earthTex");
	GLuint earthBump = renderer.GetTexture("earthBump");

	sphere = Mesh::LoadFromMeshFile("Sphere.msh");
	cube = Mesh::LoadFromMeshFile("Cube.msh");
	quad = Mesh::GenerateQuad();
	heightmapQuad = Mesh::GeneratePatchQuad(heightMapWorldSize);

	heightMapMaster = new PHeightMapMasterNode(renderer, camera);
	AddChild(heightMapMaster);

	pointLightsNode = new SceneNode(renderer);
	//AddChild(pointLightsNode);

	SceneNode* pointLightNode = new PointLightNode(
		renderer,
		Vector3(10.0f, 100.0f, 0.0f),
		Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		1000.0f
	);
	pointLightsNode->AddChild(pointLightNode);
	pointLightNode->lightMesh = sphere;

	spotLightsNode = new SceneNode(renderer);
	//AddChild(spotLightsNode);

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
	orbiterNodes->transform = Matrix4::Translation(Vector3(-200.0f, 100.0f, -200.0f));
	for (int i = 0; i < 10; i++) {
		SceneNode* orbitPoint = new SceneNode(renderer);
		orbiterNodes->AddChild(orbitPoint);
		orbitPoint->transform = Matrix4::Translation(Vector3(rand() % 400, (float)(rand() % 20), rand() % 400));
		OrbiterNode* orbiter = new OrbiterNode(renderer, 1.0f + (rand() % 20), -10.0f + (rand() % 20));
		orbitPoint->AddChild(orbiter);
		orbiter->modelScale = Vector3(1.0f, 1.0f, 1.0f) * (3.0f + (rand() % 7));
		orbiter->mesh = sphere;
		orbiter->diffuseTex = earthTex;
		orbiter->bumpTex = earthBump;
	}
}

DemoSceneNode::~DemoSceneNode() {
	delete sphere;
	delete cube;
	delete quad;
	delete heightmapQuad;
}

void DemoSceneNode::Update(float dt) {
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1))
		heightMapMaster->paused = !heightMapMaster->paused;
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) {
		renderMode = (renderMode + 1) % 3;
		renderer.SetShaderUniformi("combine", "mode", renderMode);
		renderer.SetPostProcess(renderMode == 0);
	}
	SceneNode::Update(dt);
	static float offset = 0.0;
	offset += dt;
	renderer.SetShaderUniformf("skybox", "cirrus", std::sin(offset) * 0.5 + 0.5);
	renderer.SetShaderUniformf("skybox", "cumulus", std::sin(offset * 0.5) * 0.5 + 0.5);
}
