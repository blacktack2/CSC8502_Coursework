#pragma once
#include "Matrix3.h"
#include "Matrix4.h"

#include <vector>

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

	bool autoMove = true;
	float autoTime = 0.0f;

	const float AUTO_SPEED = 2.0f;

	struct KeyFrame {
		float time;
		Vector3 position;
		float roll;
		float pitch;
		float yaw;
	};
	const std::vector<KeyFrame> KEY_FRAMES = std::vector<KeyFrame>{
		{0.0f, Vector3(-3.0f, 56.0f, 21.0f), 0.0f, 21.0f, -20.0f},
		{3.0f, Vector3(14.0f, 80.0f, -14.0f), 0.0f, 17.0f, -20.0f},
		{4.0f, Vector3(19.0f, 80.0f, -30.0f), 0.0f, -2.0f, -10.0f},
		{6.0f, Vector3(13.0f, 74.0f, -62.0f), 0.0f, -11.0f, 25.0f},
		{7.0f, Vector3(-11.0f, 72.0f, -101.0f), 0.0f, -10.0f, 34.0f},
		{9.0f, Vector3(-41.0f, 56.0f, -130.0f), 0.0f, -17.0f, 57.0f},
		{13.0f, Vector3(-94.0f, 40.0f, -152.0f), 0.0f, -1.0f, 73.0f},
		{15.0f, Vector3(-115.0f, 52.0f, -157.0f), 0.0f, 27.0f, 76.0f},
		{17.0f, Vector3(-229.0f, 72.0f, -186.0f), 0.0f, -7.0f, 71.0f},
		{24.0f, Vector3(-326.0f, 72.0f, -248.0f), 0.0f, -4.0f, 51.0f},
		{30.0f, Vector3(-421.0f, 107.0f, -281.0f), 0.0f, -3.0f, 83.0f},
		{50.0f, Vector3(-1553.0f, 107.0f, 205.0f), 0.0f, 30.0f, 95.0f},
		{70.0f, Vector3(-2523.0f, 107.0f, -567.0f), 0.0f, -7.0f, 68.0f},
	};
};
