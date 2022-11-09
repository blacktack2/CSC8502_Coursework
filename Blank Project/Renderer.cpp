#include "Renderer.h"

#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{
	camera = new Camera();

	heightMap = new HeightMap(TEXTUREDIR"noise.png");
	camera->SetPosition(heightMap->GetSize() * Vector3(0.5f, 2.0f, 0.5f));

	shader = new Shader("basic.vert", "basic.frag");

	if(!shader->LoadSuccess())
		return;

	earthTex = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (!earthTex)
		return;

	SetTextureRepeating(earthTex, true);

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	init = true;
}
Renderer::~Renderer(void)	{
	delete camera;

	delete heightMap;
	delete shader;
}

void Renderer::UpdateScene(float dt) {
	camera->Update(dt);
	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();
}

void Renderer::RenderScene()	{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	BindShader(shader);

	SetUniform1i("diffuseTex", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTex);

	heightMap->Draw();
}
