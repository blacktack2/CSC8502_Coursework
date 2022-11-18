#include "Renderer.h"

#include "DemoSceneNode.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Light.h"
#include "../nclgl/SceneNode.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent) {
	AddTexture("earthTex", SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG"    , SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	AddTexture("earthBump", SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	AddTexture("blankTex", SOIL_load_OGL_texture(TEXTUREDIR"blank.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	if (!GetTexture("earthTex") || !GetTexture("earthBump") || !GetTexture("blankTex"))
		return;

	OGLRenderer::SetTextureRepeating(GetTexture("earthTex"), true);
	OGLRenderer::SetTextureRepeating(GetTexture("earthBump"), true);
	OGLRenderer::SetTextureRepeating(GetTexture("blankTex"), true);

	identityMatrix.ToIdentity();
	cameraProjMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	camera = new Camera();

	skyboxShader = new Shader("skybox.vert", "skybox.frag");

	sceneShader = new Shader("buffer.vert", "buffer.frag");
	heightmapShader = new Shader("heightmap.vert", "heightmap.frag", "", "heightmap.tesc", "heightmap.tese");

	depthShader = new Shader("depth.vert", "depth.frag");
	heightmapDepthShader = new Shader("heightmapDepth.vert", "heightmapDepth.frag", "", "heightmapDepth.tesc", "heightmapDepth.tese");

	lightShader = new Shader("light.vert", "light.frag");

	combineShader = new Shader("combine.vert", "combine.frag");

	fogShader = new Shader("fog.vert", "fog.frag");
	blurShader = new Shader("blur.vert", "blur.frag");

	presentShader = new Shader("basic.vert", "basic.frag");

	noiseShader = new Shader("noise.vert", "noise.frag");

	AddShader("skybox", skyboxShader);

	AddShader("scene", sceneShader);
	AddShader("heightmap", heightmapShader);

	AddShader("depth", depthShader);
	AddShader("heightmapDepth", heightmapDepthShader);

	AddShader("light", lightShader);

	AddShader("combine", combineShader);

	AddShader("blur", blurShader);
	AddShader("fog", fogShader);

	AddShader("present", presentShader);

	AddShader("noise", noiseShader);

	if(!skyboxShader->LoadSuccess() ||
		!sceneShader->LoadSuccess() || !heightmapShader->LoadSuccess() ||
		!depthShader->LoadSuccess() || !heightmapDepthShader->LoadSuccess() ||
		!lightShader->LoadSuccess() ||
		!combineShader->LoadSuccess() ||
		!fogShader->LoadSuccess() || !blurShader->LoadSuccess() ||
		!presentShader->LoadSuccess() ||
		!noiseShader->LoadSuccess())
		return;

	quad = Mesh::GenerateQuad();

	root = new DemoSceneNode(*this, *camera);

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &shadowFBO);
	glGenFramebuffers(1, &lightFBO);
	glGenFramebuffers(1, &combineFBO);
	glGenFramebuffers(1, &blurFBO);
	glGenFramebuffers(1, &fogFBO);

	const GLenum buffers[3] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2
	};

	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(shadowTex, true, true);
	GenerateScreenTexture(lightDiffuseTex);
	GenerateScreenTexture(lightSpecularTex);
	GenerateScreenTexture(sceneTex1);
	GenerateScreenTexture(sceneTex2);

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

	glBindFramebuffer(GL_FRAMEBUFFER, combineFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTex1, 0);
	glDrawBuffers(1, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, blurFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTex1, 0);
	glDrawBuffers(1, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, fogFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTex2, 0);
	glDrawBuffers(1, buffers);

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

	BindShader(heightmapDepthShader);
	glUniform1i(UniformLocation("heightTex"), 2);

	BindShader(lightShader);
	glUniform1i(UniformLocation("depthTex"), 0);
	glUniform1i(UniformLocation("normTex"), 1);
	glUniform1i(UniformLocation("shadowTex"), 3);

	BindShader(combineShader);
	glUniform1i(UniformLocation("diffuseTex"), 0);
	glUniform1i(UniformLocation("diffuseLight"), 1);
	glUniform1i(UniformLocation("specularLight"), 2);
	glUniform1i(UniformLocation("normTex"), 3);
	glUniform1i(UniformLocation("depthTex"), 4);
	glUniform3f(UniformLocation("ambientColour"), 0.0f, 0.0f, 0.0f);
	glUniform1i(UniformLocation("mode"), 0);

	BindShader(blurShader);
	glUniform1i(UniformLocation("sceneTex"), 0);

	BindShader(fogShader);
	glUniform1i(UniformLocation("sceneTex"), 0);
	glUniform1i(UniformLocation("depthTex"), 1);
	glUniform1f(UniformLocation("fogMin"), 0.99232f);
	glUniform1f(UniformLocation("fogMax"), 0.99717f);
	glUniform3f(UniformLocation("fogColour"), 1.0f, 1.0f, 1.0f);

	BindShader(presentShader);
	glUniform1i(UniformLocation("sceneTex"), 0);

	SetPostProcess(true);
	init = true;
}

Renderer::~Renderer(void) {
	delete camera;

	delete quad;

	delete root;

	delete skyboxShader;

	delete sceneShader;
	delete heightmapShader;

	delete depthShader;
	delete heightmapDepthShader;

	delete lightShader;

	delete combineShader;

	delete blurShader;
	delete fogShader;

	delete presentShader;

	delete noiseShader;

	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightDiffuseTex);
	glDeleteTextures(1, &lightSpecularTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &shadowFBO);
	glDeleteFramebuffers(1, &lightFBO);
	glDeleteFramebuffers(1, &fogFBO);
	glDeleteFramebuffers(1, &blurFBO);
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
	if (postProcess) {
		ApplyBlur();
		DrawFog();
	}
	PresentScene();

	ClearNodeLists();
}

void Renderer::UpdateScene(float dt) {
	camera->Update(dt);
	cameraViewMatrix = camera->BuildViewMatrix();
	root->Update(dt);

	BindShader(skyboxShader);
	glUniform1f(UniformLocation("time"), window.GetTimer()->GetTotalTimeSeconds());
}

void Renderer::SetPostProcess(bool pp) {
	OGLRenderer::SetPostProcess(pp);
	if (postProcess) {
		sceneTexOut = sceneTex2;
	} else {
		sceneTexOut = sceneTex1;
	}
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

	BindShader(depthShader);

	glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	BindShader(lightShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	glUniform3fv(UniformLocation("cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform2f(UniformLocation("pixelSize"), 1.0f / width, 1.0f / height);

	Matrix4 invViewProj = (cameraProjMatrix * cameraViewMatrix).Inverse();
	glUniformMatrix4fv(UniformLocation("inverseProjView"), 1, false, invViewProj.values);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	DrawNodeLights(root);
}

void Renderer::CombineBuffers() {
	glBindFramebuffer(GL_FRAMEBUFFER, combineFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	BindShader(combineShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightDiffuseTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	quad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::ApplyBlur() {
	glBindFramebuffer(GL_FRAMEBUFFER, blurFBO);

	BindShader(blurShader);

	glActiveTexture(GL_TEXTURE0);
	for (int i = 0; i < 4; i++) {
		glBindTexture(GL_TEXTURE_2D, sceneTex1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTex2, 0);
		glUniform1i(UniformLocation("horizontal"), 0);

		quad->Draw();

		glBindTexture(GL_TEXTURE_2D, sceneTex2);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTex1, 0);
		glUniform1i(UniformLocation("horizontal"), 1);

		quad->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawFog() {
	glBindFramebuffer(GL_FRAMEBUFFER, fogFBO);

	BindShader(fogShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneTex1);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	quad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::PresentScene() {
	BindShader(presentShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, postProcess ? sceneTex2 : sceneTex1);

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
		BindShader(depthShader);
		Matrix4 depthProjViewMatrix;
		Matrix4 depthProjMatrix;
		Matrix4 depthViewMatrix;
		if (node->light->position.w == 0.0f) {
			depthViewMatrix = Matrix4::BuildViewMatrix((node->light->direction * 100), Vector3());
			depthProjMatrix = Matrix4::Orthographic(-1.0f, 10000.0f, -1000.0f, 1000.0f, -1000.0f, 1000.0f);
		} else {
			depthViewMatrix = Matrix4::BuildViewMatrix(node->light->position.ToVector3(), Vector3());
			depthProjMatrix = Matrix4::Perspective(1.0f, 10000.0f, 1.0f, 170.0f);
		}
		depthProjViewMatrix = depthProjMatrix * depthViewMatrix;

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		PushViewport(Vector4(0, 0, SHADOWSIZE, SHADOWSIZE));

		DrawNodeDepth(root, depthProjViewMatrix);

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		PopViewport();

		glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
		BindShader(lightShader);
		SetShaderLight(node->light);
		Matrix4 shadowMatrix = depthProjMatrix * depthViewMatrix;
		glUniformMatrix4fv(UniformLocation("shadowMatrix"), 1, false, shadowMatrix.values);
		glUniformMatrix4fv(UniformLocation("viewMatrix"), 1, false, cameraViewMatrix.values);
		glUniformMatrix4fv(UniformLocation("projMatrix"), 1, false, cameraProjMatrix.values);

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

void Renderer::DrawNodeDepth(SceneNode* node, Matrix4& projViewMatrix) {
	if (!node->GetDepthShader())
		BindShader(depthShader);
	else
		BindShader(node->GetDepthShader());
	glUniformMatrix4fv(UniformLocation("projViewMatrix"), 1, false, projViewMatrix.values);
	node->DrawMeshDepth();
	for (auto child : *node)
		DrawNodeDepth(child, projViewMatrix);
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
