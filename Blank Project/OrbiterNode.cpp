#include "OrbiterNode.h"

#include <algorithm>

void OrbiterNode::Update(float dt) {
	offset += dt * speed;
	transform = Matrix4::Translation(Vector3(std::cos(offset), 0.0f, std::sin(offset)) * radius);
	SceneNode::Update(dt);
}
