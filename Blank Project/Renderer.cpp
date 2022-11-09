#include "Renderer.h"

#include "../nclgl/Camera.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{
	triangle = Mesh::GenerateTriangle();

	basicShader = new Shader("basic.vert", "basic.frag");

	if(!basicShader->LoadSuccess())
		return;

	camera = new Camera();

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	modelMatrix = Matrix4::Translation(Vector3(0.0f, 0.0f, -1500.0f)) * Matrix4::Rotation(0.0f, Vector3(0.0f, 1.0f, 0.0f)) * Matrix4::Scale(Vector3(150.0f, 150.0f, 150.0f));

	init = true;
}
Renderer::~Renderer(void)	{
	delete camera;

	delete triangle;
	delete basicShader;
}

void Renderer::UpdateScene(float dt) {
	camera->Update(dt);
	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();
}

void Renderer::RenderScene()	{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);	

	BindShader(basicShader);
	triangle->Draw();
}
