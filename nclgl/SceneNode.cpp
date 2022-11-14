#include "SceneNode.h"

#include <algorithm>

SceneNode::SceneNode(OGLRenderer& renderer) : renderer(renderer) {
}

SceneNode::~SceneNode() {
	ClearChildren();
	delete light;
}

void SceneNode::AddChild(SceneNode* child) {
	if (child == this) return;
	children.push_back(child);
	child->parent = this;
}

void SceneNode::RemoveChild(int childIndex) {
	if (childIndex > children.size()) return;
	delete children[childIndex];
	children.erase(children.begin() + childIndex);
}

void SceneNode::RemoveChild(SceneNode* child) {
	children.erase(std::remove_if(children.begin(), children.end(), [child](SceneNode* c) {
		if (c == child) {
			delete child;
			return true;
		}
		return false;
		}), children.end());
}

void SceneNode::ClearChildren() {
	for (auto child : children)
		delete child;
	children.clear();
}

void SceneNode::Update(float dt) {
	worldTransform = parent ? parent->worldTransform * transform : transform;
	for (auto child : children)
		child->Update(dt);
}

void SceneNode::DrawMesh(const OGLRenderer& renderer) {
	if (mesh)
		mesh->Draw();
}

void SceneNode::DrawLight(const OGLRenderer& renderer) {
	if (lightMesh)
		lightMesh->Draw();
}
