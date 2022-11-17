#pragma once
#include "../nclgl/Frustum.h"
#include "../nclgl/OGLRenderer.h"

#include <vector>

class Camera;
class Light;

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	 ~Renderer(void);

	 void RenderScene() override;
	 void UpdateScene(float msec) override;
protected:
	void DrawSkybox();
	void FillBuffers();
	void DrawLights();
	void CombineBuffers();
	void DrawFog();
	void PresentScene();

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();

	void DrawNodeAlbedos();
	void DrawNodeAlbedo(SceneNode* node);

	void DrawNodeLights(SceneNode* node);

	void DrawNodeDepth(SceneNode* node, Matrix4& projViewMatrix);

	void GenerateScreenTexture(GLuint& into, bool depth = false, bool shadow = false);

	Camera* camera = nullptr;

	Mesh* quad = nullptr;

	SceneNode* root = nullptr;
	std::vector<SceneNode*> nodeList{};
	std::vector<SceneNode*> nodeListTransparent{};

	Frustum frameFrustum;

	Shader* skyboxShader = nullptr;

	Shader* sceneShader = nullptr;
	Shader* heightmapShader = nullptr;

	Shader* depthShader = nullptr;
	Shader* heightmapDepthShader = nullptr;

	Shader* lightShader = nullptr;

	Shader* combineShader = nullptr;

	Shader* fogShader = nullptr;

	Shader* presentShader = nullptr;

	Shader* noiseShader = nullptr;

	Matrix4 cameraViewMatrix;
	Matrix4 cameraProjMatrix;
	Matrix4 identityMatrix;

	GLuint bufferFBO = 0;
	GLuint shadowFBO = 0;
	GLuint lightFBO = 0;
	GLuint combineFBO = 0;
	GLuint fogFBO = 0;

	GLuint bufferColourTex = 0;
	GLuint bufferNormalTex = 0;
	GLuint bufferDepthTex = 0;

	GLuint shadowTex = 0;

	GLuint lightDiffuseTex = 0;
	GLuint lightSpecularTex = 0;

	GLuint sceneTex1 = 0;
	GLuint sceneTex2 = 0;

	const GLsizei SHADOWSIZE = 2048;
};
