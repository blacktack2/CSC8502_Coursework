#include "ProceduralHeightMap.h"

ProceduralHeightMap::ProceduralHeightMap(OGLRenderer& renderer, Mesh* quad, int x, int z) : renderer(renderer), quad(quad), offsetX(x), offsetZ(z) {
	glGenFramebuffers(1, &generationFBO);
}

ProceduralHeightMap::~ProceduralHeightMap() {
	DeGenerate();
	glDeleteFramebuffers(1, &generationFBO);
}

void ProceduralHeightMap::Generate() {
	glBindFramebuffer(GL_FRAMEBUFFER, generationFBO);

	GLuint bufferTex1, bufferTex2;
	glGenTextures(1, &bufferTex1);
	glGenTextures(1, &bufferTex2);
	glBindTexture(GL_TEXTURE_2D, bufferTex1);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, resolutionX * worldX, resolutionZ * worldZ, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glBindTexture(GL_TEXTURE_2D, bufferTex2);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, resolutionX * worldX, resolutionZ * worldZ, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glBindTexture(GL_TEXTURE_2D, 0);


	Shader* noiseShader = renderer.GetShader("noise");
	glUseProgram(noiseShader->GetProgram());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferTex1);

	renderer.PushClearColour(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferTex1, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferTex2, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	renderer.PopClearColour();

	float relScaleX = resolutionX * 0.01f;
	float relScaleZ = resolutionZ * 0.01f;

	renderer.PushViewport(Vector4(0, 0, resolutionX * worldX, resolutionZ * worldZ));

	renderer.SetShaderUniformi("noise", "readTex", 0);
	renderer.SetShaderUniformf("noise", "frequency", 1.0f);
	renderer.SetShaderUniformf("noise", "amplitude", 0.5f);
	renderer.SetShaderUniformf("noise", "offset", relScaleX * offsetX, -relScaleZ * offsetZ);
	renderer.SetShaderUniformf("noise", "scale", relScaleX, relScaleZ);
	quad->Draw();
	glBindTexture(GL_TEXTURE_2D, bufferTex2);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferTex1, 0);
	renderer.SetShaderUniformf("noise", "frequency", 2.0f);
	renderer.SetShaderUniformf("noise", "amplitude", 0.35f);
	renderer.SetShaderUniformf("noise", "offset", relScaleX * offsetX * 2.0f, -relScaleZ * offsetZ * 2.0f);
	renderer.SetShaderUniformf("noise", "scale", relScaleX, relScaleZ);
	quad->Draw();
	glBindTexture(GL_TEXTURE_2D, bufferTex1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferTex2, 0);
	renderer.SetShaderUniformf("noise", "frequency", 4.0f);
	renderer.SetShaderUniformf("noise", "amplitude", 0.15f);
	renderer.SetShaderUniformf("noise", "offset", relScaleX * offsetX * 4.0f, -relScaleZ * offsetZ * 4.0f);
	renderer.SetShaderUniformf("noise", "scale", relScaleX, relScaleZ);
	quad->Draw();

	renderer.PopViewport();

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteTextures(1, &bufferTex1);
	heightTex = bufferTex2;
}

void ProceduralHeightMap::DeGenerate() {
	if (heightTex)
		glDeleteTextures(1, &heightTex);
}
