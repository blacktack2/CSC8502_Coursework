#pragma once
#include "Matrix3.h"
#include "Matrix4.h"

class Camera {
public:
	Camera(float roll = 0.0f, float pitch = 0.0f, float yaw = 0.0f, Vector3 position = Vector3(0.0f, 0.0f, 0.0f));
	~Camera() = default;

	virtual void Update(float dt = 1.0f);

	virtual Matrix4 BuildViewMatrix();

	inline float GetRoll() { return roll; }
	inline void SetRoll(float r) { roll = r; }

	inline float GetPitch() { return pitch; }
	inline void SetPitch(float p) { pitch = p; }

	inline float GetYaw() { return yaw; }
	inline void SetYaw(float y) { yaw = y; }

	inline Vector3 GetPosition() { return position; }
	inline void SetPosition(Vector3 p) { position = p; }
protected:
	float roll;
	float pitch;
	float yaw;

	Vector3 position;
};
