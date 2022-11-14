#include "OrbiterNode.h"

#include <algorithm>

OrbiterNode::OrbiterNode(OGLRenderer& renderer, float radius, float speed) : SceneNode(renderer), radius(radius), speed(speed) {
}

void OrbiterNode::Update(float dt) {
	offset += dt * speed;
	transform = Matrix4::Translation(Vector3(std::cos(offset), 0.0f, std::sin(offset)) * radius);
	SceneNode::Update(dt);
}
