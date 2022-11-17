#include "SunNode.h"

#include "../nclgl/Light.h"
#include "../nclgl/LightNode.h"

SunNode::SunNode(OGLRenderer& renderer, Mesh* quad) : SceneNode(renderer) {
	sunLightNode = new DirectionLightNode(renderer, Vector3(1.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	AddChild(sunLightNode);
	sunLightNode->lightMesh = quad;
}

SunNode::~SunNode() {
}

void SunNode::Update(float dt) {
	SceneNode::Update(dt);

	currentTime += dt * TIME_SCALE;
	currentTime = currentTime > 24.0f ? currentTime - 24.0f : currentTime;
	//currentTime = 14.0f;

	sunLightNode->light->direction = Vector3(std::cos((currentTime + 18.0f) * 6.28318530718f / 24.0f), std::sin((currentTime + 18.0f) * 6.28318530718f / 24.0f), 0.1f).Normalised();

	for (int i = 1; i < AMBIENCE.size(); i++) {
		if (currentTime < AMBIENCE[i].first) {
			Vector3 ambience = Vector3::Lerp(AMBIENCE[i - 1].second, AMBIENCE[i].second, (currentTime - AMBIENCE[i - 1].first) / (AMBIENCE[i].first - AMBIENCE[i - 1].first));
			renderer.SetShaderUniformf("combine", "ambienceColour", ambience.x, ambience.y, ambience.z);
			break;
		}
	}
	for (int i = 1; i < LIGHT.size(); i++) {
		if (currentTime < LIGHT[i].first) {
			sunLightNode->light->colour = Vector4::Lerp(LIGHT[i - 1].second, LIGHT[i].second, (currentTime - LIGHT[i - 1].first) / (LIGHT[i].first - LIGHT[i - 1].first));
			break;
		}
	}

	renderer.SetShaderUniformf("skybox", "sunDir", sunLightNode->light->direction.x, sunLightNode->light->direction.y, sunLightNode->light->direction.z);
}
