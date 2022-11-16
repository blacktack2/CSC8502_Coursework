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

void SceneNode::DrawMesh() {
	if (mesh) {
		Matrix4 model = GetWorldTransform() * Matrix4::Scale(modelScale);
		glUniformMatrix4fv(renderer.UniformLocation("modelMatrix"), 1, false, model.values);

		PreDrawMesh();

		mesh->Draw();
	}
}

void SceneNode::DrawMeshDepth() {
	if (mesh && occluder) {
		Matrix4 model = GetWorldTransform() * Matrix4::Scale(modelScale);
		glUniformMatrix4fv(renderer.UniformLocation("modelMatrix"), 1, false, model.values);

		PreDrawMeshDepth();

		mesh->Draw();
	}
}

void SceneNode::DrawLight() {
	if (lightMesh)
		lightMesh->Draw();
}

void SceneNode::PreDrawMesh() {
	if (diffuseTex) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTex);
	}

	if (bumpTex) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, bumpTex);
	}
}

void SceneNode::PreDrawMeshDepth() {
}
