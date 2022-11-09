#pragma once
#include "../NCLGL/OGLRenderer.h"

class Camera;
class HeightMap;
class Light;

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	 ~Renderer(void);

	 void RenderScene() override;
	 void UpdateScene(float msec) override;
protected:
	void FillBuffers();
	void DrawPointLights();
	void CombineBuffers();

	void GenerateScreenTexture(GLuint& into, bool depth = false);

	Camera* camera = nullptr;

	HeightMap* heightMap = nullptr;
	Mesh* sphere = nullptr;
	Mesh* quad = nullptr;

	Shader* sceneShader = nullptr;
	Shader* pointLightShader = nullptr;
	Shader* combineShader = nullptr;

	Light* pointLights = nullptr;

	GLuint bufferFBO = 0;
	GLuint pointLightFBO = 0;

	GLuint bufferColourTex = 0;
	GLuint bufferNormalTex = 0;
	GLuint bufferDepthTex = 0;

	GLuint lightDiffuseTex = 0;
	GLuint lightSpecularTex = 0;

	GLuint earthTex = 0;
	GLuint earthBump = 0;

	const int NUM_LIGHTS = 32;
};
