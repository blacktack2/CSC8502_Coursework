#include "Frustum.h"

#include "SceneNode.h"
#include "Matrix4.h"

void Frustum::FromMatrix(const Matrix4& mvp) {
	Vector3 xaxis(mvp.values[0], mvp.values[4], mvp.values[8]);
	Vector3 yaxis(mvp.values[1], mvp.values[5], mvp.values[9]);
	Vector3 zaxis(mvp.values[2], mvp.values[6], mvp.values[10]);
	Vector3 waxis(mvp.values[3], mvp.values[7], mvp.values[11]);

	planes[0] = Plane(waxis - xaxis, (mvp.values[15] - mvp.values[12]), true);
	planes[1] = Plane(waxis + xaxis, (mvp.values[15] + mvp.values[12]), true);
	planes[2] = Plane(waxis + yaxis, (mvp.values[15] + mvp.values[13]), true);
	planes[3] = Plane(waxis - yaxis, (mvp.values[15] - mvp.values[13]), true);
	planes[4] = Plane(waxis + zaxis, (mvp.values[15] + mvp.values[14]), true);
	planes[5] = Plane(waxis - zaxis, (mvp.values[15] - mvp.values[14]), true);
}

bool Frustum::IsInside(SceneNode& node) {
	for (auto& plane : planes)
		if (node.boundingRadius > 0.0f && !plane.SphereInside(node.GetWorldTransform().GetPositionVector(), node.boundingRadius))
			return false;
	return true;
}
