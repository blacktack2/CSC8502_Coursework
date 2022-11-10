#include "Camera.h"
#include "Window.h"

#include <algorithm>

Camera::Camera(float roll_, float pitch_, float yaw_, Vector3 position_) :
roll(roll_), pitch(pitch_), yaw(yaw_), position(position_) {
}

void Camera::Update(float dt) {
	float speed = (Window::GetKeyboard()->KeyDown(KEYBOARD_CONTROL) ? 200.0f : 30.0f) * dt;
	float rollSpeed = 30.0f * dt;

	roll += (Window::GetKeyboard()->KeyDown(KEYBOARD_Q) ? rollSpeed : 0.0f) - (Window::GetKeyboard()->KeyDown(KEYBOARD_E) ? rollSpeed : 0.0f);
	roll += (roll < 0.0f) ? 360.0f : ((roll >= 360.0f) ? -360.0f : 0.0f);

	pitch = std::min(std::max(pitch - Window::GetMouse()->GetRelativePosition().y, -90.0f), 90.0f);

	yaw -= Window::GetMouse()->GetRelativePosition().x;
	yaw += (yaw < 0.0f) ? 360.0f : ((yaw >= 360.0f) ? -360.0f : 0.0f);

	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0.0f, 1.0f, 0.0f));
	Vector3 forward = rotation * Vector3(0.0f, 0.0f, -1.0f);
	Vector3 right = rotation * Vector3(1.0f, 0.0f, 0.0f);

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W))
		position += forward * speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_S))
		position -= forward * speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A))
		position -= right * speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D))
		position += right * speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE))
		position.y += speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT))
		position.y -= speed;
}

Matrix4 Camera::BuildViewMatrix() {
	return Matrix4::Rotation(-roll, Vector3(0.0f, 0.0f, 1.0f)) *
		   Matrix4::Rotation(-pitch, Vector3(1.0f, 0.0f, 0.0f)) *
		   Matrix4::Rotation(-yaw, Vector3(0.0f, 1.0f, 0.0f)) *
		   Matrix4::Translation(-position);
}
