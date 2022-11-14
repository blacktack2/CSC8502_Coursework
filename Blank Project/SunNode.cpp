#include "SunNode.h"

#include "../nclgl/Light.h"
#include "../nclgl/LightNode.h"

SunNode::SunNode(OGLRenderer& renderer, Shader* combinePassShader, Mesh* quad) : SceneNode(renderer), combinePassShader(combinePassShader) {
	sunLightNode = new DirectionLightNode(renderer, Vector3(1.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	AddChild(sunLightNode);
	sunLightNode->lightMesh = quad;
}

SunNode::~SunNode() {
}

void SunNode::Update(float dt) {
	SceneNode::Update(dt);

	currentTime += dt * TIME_SCALE;
	currentTime = currentTime > MIDN ? currentTime - MIDN : currentTime;

	sunLightNode->light->direction = Vector3(std::cos(currentTime * 6.28318530718 / MIDN), std::sin(currentTime * 6.28318530718 / MIDN), 0.0f).Normalised();

	Vector3 currentAmbience;
	if (currentTime < DAWN) {
		currentAmbience = Vector3::Lerp(AMBIENCE_MIDN, AMBIENCE_DAWN, currentTime / DAWN);
		sunLightNode->light->colour = Vector4::Lerp(LIGHT_MIDN, LIGHT_DAWN, currentTime / DAWN);
	} else if (currentTime < NOON) {
		currentAmbience = Vector3::Lerp(AMBIENCE_DAWN, AMBIENCE_NOON, (currentTime - DAWN) / (NOON - DAWN));
		sunLightNode->light->colour = Vector4::Lerp(LIGHT_DAWN, LIGHT_NOON, (currentTime - DAWN) / (NOON - DAWN));
	} else if (currentTime < DUSK) {
		currentAmbience = Vector3::Lerp(AMBIENCE_NOON, AMBIENCE_DUSK, (currentTime - NOON) / (DUSK - NOON));
		sunLightNode->light->colour = Vector4::Lerp(LIGHT_NOON, LIGHT_DUSK, (currentTime - NOON) / (DUSK - NOON));
	} else {
		currentAmbience = Vector3::Lerp(AMBIENCE_DUSK, AMBIENCE_MIDN, (currentTime - DUSK) / (MIDN - DUSK));
		sunLightNode->light->colour = Vector4::Lerp(LIGHT_DUSK, LIGHT_MIDN, (currentTime - DUSK) / (MIDN - DUSK));
	}

	renderer.BindShader(combinePassShader);
	glUniform3f(renderer.UniformLocation("ambientColour"), currentAmbience.x, currentAmbience.y, currentAmbience.z);
}
