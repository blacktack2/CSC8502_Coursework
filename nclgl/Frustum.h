#pragma once
#include "Plane.h"

class SceneNode;
class Matrix4;

class Frustum {
public:
	Frustum() = default;
	~Frustum() = default;

	void FromMatrix(const Matrix4& mvp);
	bool IsInside(SceneNode& node);
protected:
	Plane planes[6];
};

