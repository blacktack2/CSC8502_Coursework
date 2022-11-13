#include "Renderer.h"

#include "DemoSceneNode.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Light.h"
#include "../nclgl/SceneNode.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent) {
	camera = new Camera();

	camera->SetPosition(Vector3(0.0f, 400.0f, 0.0f));

	sceneShader = new Shader("buffer.vert", "buffer.frag");
	shadowShader = new Shader("shadow.vert", "shadow.frag");
	lightShader = new Shader("light.vert", "light.frag");
	combineShader = new Shader("combine.vert", "combine.frag");

	if(!sceneShader->LoadSuccess() || !shadowShader->LoadSuccess() || !lightShader->LoadSuccess() || !combineShader->LoadSuccess())
		return;

	quad = Mesh::GenerateQuad();

	root = new DemoSceneNode();

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &shadowFBO);
	glGenFramebuffers(1, &lightFBO);

	const GLenum buffers[3] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1
	};

	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(shadowTex, true, true);
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
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightDiffuseTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	BindShader(combineShader);
	glUniform3f(UniformLocation("ambientColour"), 0.1f, 0.1f, 0.2f);

	init = true;
}
Renderer::~Renderer(void) {
	delete camera;

	delete quad;

	delete root;

	delete sceneShader;
	delete lightShader;
	delete combineShader;

	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
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

	frameFrustum.FromMatrix(projMatrix * viewMatrix);

	FillBuffers();
	DrawLights();
	CombineBuffers();

	ClearNodeLists();
}

void Renderer::UpdateScene(float dt) {
	camera->Update(dt);
	root->Update(dt);
}

void Renderer::FillBuffers() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindShader(sceneShader);
	glUniform1i(UniformLocation("diffuseTex"), 0);
	glUniform1i(UniformLocation("bumpTex"), 1);

	modelMatrix.ToIdentity();
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	UpdateShaderMatrices();

	DrawNodeAlbedos();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawLights() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	BindShader(shadowShader);

	glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	BindShader(lightShader);

	glUniform1i(UniformLocation("depthTex"), 0);
	glUniform1i(UniformLocation("normTex"), 1);
	glUniform1i(UniformLocation("shadowTex"), 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	glUniform3fv(UniformLocation("cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform2f(UniformLocation("pixelSize"), 1.0f / width, 1.0f / height);

	Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(UniformLocation("inverseProjView"), 1, false, invViewProj.values);
	UpdateShaderMatrices();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	DrawNodeLights(root);
}

void Renderer::CombineBuffers() {
	BindShader(combineShader);

	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glUniform1i(UniformLocation("diffuseTex"), 0);
	glUniform1i(UniformLocation("diffuseLight"), 1);
	glUniform1i(UniformLocation("specularLight"), 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightDiffuseTex);

	glActiveTexture(GL_TEXTURE2);
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
	if (!node->mesh) return;
	Matrix4 model = node->GetWorldTransform() * Matrix4::Scale(node->modelScale);
	glUniformMatrix4fv(UniformLocation("modelMatrix"), 1, false, model.values);

	if (node->diffuseTex) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, node->diffuseTex);
	}

	if (node->bumpTex) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, node->bumpTex);
	}

	node->DrawMesh(*this);
}

void Renderer::DrawNodeLights(SceneNode* node) {
	if (node->light && node->lightMesh) {
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		BindShader(shadowShader);
		Matrix4 shadowProjMatrix;
		Matrix4 shadowViewMatrix;
		if (node->light->position.w == 0.0f) {
			//shadowViewMatrix = Matrix4::BuildViewMatrix((node->light->direction * -10), Vector3());
			shadowViewMatrix = Matrix4::BuildViewMatrix(Vector3(10.0f, 1000.0f, 10.0f), Vector3());
			shadowProjMatrix = Matrix4::Orthographic(-1.0f, 10000.0f, -1000.0f, 1000.0f, -1000.0f, 1000.0f);
		} else {
			shadowViewMatrix = Matrix4::BuildViewMatrix(node->light->position.xyz(), Vector3());
			shadowProjMatrix = Matrix4::Perspective(1.0f, 10000.0f, 1.0f, 170.0f);
		}
		glUniformMatrix4fv(UniformLocation("viewMatrix"), 1, false, shadowViewMatrix.values);
		glUniformMatrix4fv(UniformLocation("projMatrix"), 1, false, shadowProjMatrix.values);

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

		DrawNodeShadows(root);

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glViewport(0, 0, width, height);

		glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
		BindShader(lightShader);
		UpdateShaderMatrices();
		SetShaderLight(node->light);
		shadowMatrix = shadowProjMatrix * shadowViewMatrix;
		glUniformMatrix4fv(UniformLocation("shadowMatrix"), 1, false, shadowMatrix.values);

		glBlendFunc(GL_ONE, GL_ONE);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_ALWAYS);
		glDepthMask(GL_FALSE);

		node->DrawLight(*this);

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
	if (node->mesh) {
		Matrix4 model = node->GetWorldTransform() * Matrix4::Scale(node->modelScale);
		glUniformMatrix4fv(UniformLocation("modelMatrix"), 1, false, model.values);

		node->DrawMesh(*this);
	}
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
