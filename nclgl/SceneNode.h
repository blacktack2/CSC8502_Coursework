#pragma once
#include "Matrix4.h"
#include "Mesh.h"
#include "OGLRenderer.h"
#include "Shader.h"
#include "Vector3.h"
#include "Vector4.h"

#include <vector>

class SceneNode {
public:
	SceneNode(OGLRenderer& renderer);
	~SceneNode();

	inline Shader* GetShader() const { return shader; }

	inline const Matrix4& GetWorldTransform() const { return worldTransform; }

	void AddChild(SceneNode* child);
	void RemoveChild(int childIndex);
	void RemoveChild(SceneNode* child);
	void ClearChildren();

	inline std::vector<SceneNode*>::const_iterator begin() const { return children.begin(); }
	inline std::vector<SceneNode*>::const_iterator end() const { return children.end(); }

	virtual void Update(float dt);
	virtual void DrawMesh(const OGLRenderer& renderer);
	virtual void DrawLight(const OGLRenderer& renderer);

	inline static bool CompareByCameraDistance(SceneNode* a, SceneNode* b) { return a->distanceFromCamera < b->distanceFromCamera; }

	Mesh* mesh = nullptr;
	Light* light = nullptr;
	Mesh* lightMesh = nullptr;

	Matrix4 transform = Matrix4();
	Vector3 modelScale = Vector3(1.0f, 1.0f, 1.0f);

	float distanceFromCamera = 0.0f;
	float boundingRadius = 0.0f;

	bool opaque = true;

	GLuint diffuseTex = 0;
	GLuint bumpTex = 0;
protected:
	OGLRenderer& renderer;
	SceneNode* parent = nullptr;

	Shader* shader = nullptr;

	Matrix4 worldTransform{};

	std::vector<SceneNode*> children{};
};
