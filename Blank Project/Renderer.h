#pragma once
#include "../NCLGL/OGLRenderer.h"

class Camera;

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	 ~Renderer(void);
	 void RenderScene() override;
	 void UpdateScene(float msec) override;
protected:
	Camera* camera = nullptr;

	Mesh* triangle = nullptr;
	Shader* basicShader = nullptr;
};
