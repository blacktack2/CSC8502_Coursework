#pragma once
#include "../NCLGL/OGLRenderer.h"

class Camera;
class HeightMap;

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	 ~Renderer(void);
	 void RenderScene() override;
	 void UpdateScene(float msec) override;
protected:
	Camera* camera = nullptr;

	HeightMap* heightMap = nullptr;
	Shader* shader = nullptr;

	GLuint earthTex;
};
