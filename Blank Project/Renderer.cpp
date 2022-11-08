#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{
	triangle = Mesh::GenerateTriangle();

	basicShader = new Shader("basic.vert", "basic.frag");

	if(!basicShader->LoadSuccess())
		return;

	init = true;
}
Renderer::~Renderer(void)	{
	delete triangle;
	delete basicShader;
}

void Renderer::UpdateScene(float dt) {

}

void Renderer::RenderScene()	{
	glClearColor(0.2f,0.2f,0.2f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);	

	BindShader(basicShader);
	triangle->Draw();
}

