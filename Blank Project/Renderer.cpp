#include "Renderer.h"

#include "DemoSceneNode.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Light.h"
#include "../nclgl/SceneNode.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent) {
	identityMatrix.ToIdentity();
	cameraProjMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	camera = new Camera();

	camera->SetPosition(Vector3(0.0f, 400.0f, 0.0f));

	skyboxShader = new Shader("skybox.vert", "skybox.frag");

	sceneShader = new Shader("buffer.vert", "buffer.frag");
	heightmapShader = new Shader("heightmap.vert", "heightmap.frag", "", "heightmap.tesc", "heightmap.tese");

	shadowShader = new Shader("shadow.vert", "shadow.frag");
	lightShader = new Shader("light.vert", "light.frag");

	combineShader = new Shader("combine.vert", "combine.frag");

	noiseShader = new Shader("noise.vert", "noise.frag");

	AddShader("skybox", skyboxShader);
	AddShader("scene", sceneShader);
	AddShader("shadow", shadowShader);
	AddShader("light", lightShader);
	AddShader("combine", combineShader);
	AddShader("heightmap", heightmapShader);

	AddShader("noise", noiseShader);

	if(!skyboxShader->LoadSuccess() ||
		!sceneShader->LoadSuccess() ||
		!heightmapShader->LoadSuccess() ||
		!shadowShader->LoadSuccess() ||
		!lightShader->LoadSuccess() ||
		!combineShader->LoadSuccess() ||
		!noiseShader->LoadSuccess())
		return;

	quad = Mesh::GenerateQuad();

	root = new DemoSceneNode(*this);

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &shadowFBO);
	glGenFramebuffers(1, &lightFBO);

	const GLenum buffers[3] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2
	};

	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(shadowTex, true, true);
	GenerateScreenTexture(lightAmbienceTex);
	GenerateScreenTexture(lightDiffuseTex);
	GenerateScreenTexture(lightSpecularTex);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightAmbienceTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightDiffuseTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(3, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	BindShader(skyboxShader);
	
	glUniformMatrix4fv(UniformLocation("projMatrix"), 1, false, cameraProjMatrix.values);

	BindShader(sceneShader);
	glUniform1i(UniformLocation("diffuseTex"), 0);
	glUniform1i(UniformLocation("bumpTex"), 1);
	glUniformMatrix4fv(UniformLocation("projMatrix"), 1, false, cameraProjMatrix.values);

	BindShader(heightmapShader);
	glUniform1i(UniformLocation("diffuseTex"), 0);
	glUniform1i(UniformLocation("bumpTex"), 1);
	glUniform1i(UniformLocation("heightTex"), 2);
	glUniformMatrix4fv(UniformLocation("projMatrix"), 1, false, cameraProjMatrix.values);

	BindShader(lightShader);

	glUniform1i(UniformLocation("depthTex"), 0);
	glUniform1i(UniformLocation("normTex"), 1);
	glUniform1i(UniformLocation("shadowTex"), 2);

	BindShader(combineShader);

	glUniform1i(UniformLocation("diffuseTex"), 0);
	glUniform1i(UniformLocation("ambienceTex"), 1);
	glUniform1i(UniformLocation("diffuseLight"), 2);
	glUniform1i(UniformLocation("specularLight"), 3);

	glUniform3f(UniformLocation("ambientColour"), 0.0f, 0.0f, 0.0f);

	init = true;
}
Renderer::~Renderer(void) {
	delete camera;

	delete quad;

	delete root;

	delete skyboxShader;
	delete sceneShader;
	delete shadowShader;
	delete lightShader;
	delete combineShader;
	delete heightmapShader;

	delete noiseShader;

	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightAmbienceTex);
	glDeleteTextures(1, &lightDiffuseTex);
	glDeleteTextures(1, &lightSpecularTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &shadowFBO);
	glDeleteFramebuffers(1, &lightFBO);
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BuildNodeLists(root);
	SortNodeLists();

	frameFrustum.FromMatrix(cameraProjMatrix * cameraViewMatrix);

	DrawSkybox();
	FillBuffers();
	DrawLights();
	CombineBuffers();

	ClearNodeLists();
}

void Renderer::UpdateScene(float dt) {
	camera->Update(dt);
	cameraViewMatrix = camera->BuildViewMatrix();
	root->Update(dt);

	BindShader(skyboxShader);
	glUniform1f(UniformLocation("time"), window.GetTimer()->GetTotalTimeSeconds());
}

void Renderer::DrawSkybox() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);

	glUniformMatrix4fv(UniformLocation("viewMatrix"), 1, false, cameraViewMatrix.values);

	quad->Draw();

	glDepthMask(GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::FillBuffers() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);

	DrawNodeAlbedos();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawLights() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	BindShader(shadowShader);

	glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	BindShader(lightShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	glUniform3fv(UniformLocation("cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform2f(UniformLocation("pixelSize"), 1.0f / width, 1.0f / height);

	Matrix4 invViewProj = (cameraProjMatrix * cameraViewMatrix).Inverse();
	glUniformMatrix4fv(UniformLocation("inverseProjView"), 1, false, invViewProj.values);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	DrawNodeLights(root);
}

void Renderer::CombineBuffers() {
	BindShader(combineShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightAmbienceTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, lightDiffuseTex);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

	quad->Draw();
}

void Renderer::BuildNodeLists(SceneNode* from) {
	if (frameFrustum.IsInside(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
		from->distanceFromCamera = Vector3::Dot(dir, dir);
		if (from->opaque)
			nodeList.push_back(from);
		else
			nodeListTransparent.push_back(from);
	}
	for (auto child : *from)
		BuildNodeLists(child);
}

void Renderer::SortNodeLists() {
	std::sort(nodeList.rbegin(), nodeList.rend(), SceneNode::CompareByCameraDistance);
	std::sort(nodeListTransparent.rbegin(), nodeListTransparent.rend(), SceneNode::CompareByCameraDistance);
}

void Renderer::ClearNodeLists() {
	nodeList.clear();
	nodeListTransparent.clear();
}

void Renderer::DrawNodeAlbedos() {
	for (const auto node : nodeList)
		DrawNodeAlbedo(node);
	for (const auto node : nodeListTransparent)
		DrawNodeAlbedo(node);
}

void Renderer::DrawNodeAlbedo(SceneNode* node) {
	if (!node->GetShader())
		BindShader(sceneShader);
	else
		BindShader(node->GetShader());
	glUniformMatrix4fv(UniformLocation("viewMatrix"), 1, false, cameraViewMatrix.values);
	node->DrawMesh();
}

void Renderer::DrawNodeLights(SceneNode* node) {
	if (node->light && node->lightMesh) {
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		BindShader(shadowShader);
		Matrix4 shadowProjMatrix;
		Matrix4 shadowViewMatrix;
		if (node->light->position.w == 0.0f) {
			shadowViewMatrix = Matrix4::BuildViewMatrix((node->light->direction * -10), Vector3());
			shadowProjMatrix = Matrix4::Orthographic(-1.0f, 10000.0f, -1000.0f, 1000.0f, -1000.0f, 1000.0f);
		} else {
			shadowViewMatrix = Matrix4::BuildViewMatrix(node->light->position.ToVector3(), Vector3());
			shadowProjMatrix = Matrix4::Perspective(1.0f, 10000.0f, 1.0f, 170.0f);
		}
		glUniformMatrix4fv(UniformLocation("viewMatrix"), 1, false, shadowViewMatrix.values);
		glUniformMatrix4fv(UniformLocation("projMatrix"), 1, false, shadowProjMatrix.values);

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		PushViewport(Vector4(0, 0, SHADOWSIZE, SHADOWSIZE));

		DrawNodeShadows(root);

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		PopViewport();

		glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
		BindShader(lightShader);
		SetShaderLight(node->light);
		Matrix4 shadowMatrix = shadowProjMatrix * shadowViewMatrix;
		glUniformMatrix4fv(UniformLocation("shadowMatrix"), 1, false, shadowMatrix.values);

		glBlendFunc(GL_ONE, GL_ONE);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_ALWAYS);
		glDepthMask(GL_FALSE);

		node->DrawLight();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glCullFace(GL_BACK);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_TRUE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	for (auto child : *node)
		DrawNodeLights(child);
}

void Renderer::DrawNodeShadows(SceneNode* node) {
	node->DrawMeshDepth();
	for (auto child : *node)
		DrawNodeShadows(child);
}

void Renderer::GenerateScreenTexture(GLuint& into, bool depth, bool shadow) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (shadow) {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	} else {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	GLuint internalFormat = shadow ? GL_DEPTH_COMPONENT : (depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8);
	GLuint format = shadow ? GL_FLOAT : GL_UNSIGNED_BYTE;
	GLuint type   = depth || shadow ? GL_DEPTH_COMPONENT : GL_RGBA;
	GLsizei texWidth  = shadow ? SHADOWSIZE : width;
	GLsizei texHeight = shadow ? SHADOWSIZE : height;
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, texWidth, texHeight, 0, type, format, nullptr);

	glBindTexture(GL_TEXTURE_2D, 0);
}
