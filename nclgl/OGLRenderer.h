#pragma once
/*
Class:OGLRenderer
Author:Rich Davison	 <richard-gordon.davison@newcastle.ac.uk>
Description:Abstract base class for the graphics tutorials. Creates an OpenGL 
3.2+ CORE PROFILE rendering context. Each lesson will create a renderer that 
inherits from this class - so all context creation is handled automatically,
but students still get to see HOW such a context is created.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*/
#include "Common.h"

#include <fstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "KHR\khrplatform.h"
#include "glad\glad.h"

#include "GL/GL.h"
#include "KHR/WGLext.h"

#include "SOIL/SOIL.h"

#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Quaternion.h"
#include "Matrix2.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include "Window.h"
#include "Shader.h"
#include "Mesh.h"

using std::vector;

#define OPENGL_DEBUGGING

extern const Matrix4 biasMatrix;

class Shader;
class Light;

class OGLRenderer	{
public:
	friend class Window;
	OGLRenderer(Window& parent);
	virtual ~OGLRenderer(void);

	virtual void RenderScene() = 0;
	virtual void UpdateScene(float msec);
	void SwapBuffers();

	bool HasInitialised() const;

	void PushClearColour(Vector4 colour);
	void PopClearColour();

	void PushViewport(Vector4 viewport);
	void PopViewport();

	void SetShaderUniformf(const std::string& shader, const std::string& uniform, float v0);
	void SetShaderUniformf(const std::string& shader, const std::string& uniform, float v0, float v1);
	void SetShaderUniformf(const std::string& shader, const std::string& uniform, float v0, float v1, float v2);
	void SetShaderUniformf(const std::string& shader, const std::string& uniform, float v0, float v1, float v2, float v3);
	void SetShaderUniformi(const std::string& shader, const std::string& uniform, int v0);
	void SetShaderUniformi(const std::string& shader, const std::string& uniform, int v0, int v1);
	void SetShaderUniformi(const std::string& shader, const std::string& uniform, int v0, int v1, int v2);
	void SetShaderUniformi(const std::string& shader, const std::string& uniform, int v0, int v1, int v2, int v3);
	void SetShaderUniform(const std::string& shader, const std::string& uniform, Matrix2 m);
	void SetShaderUniform(const std::string& shader, const std::string& uniform, Matrix3 m);
	void SetShaderUniform(const std::string& shader, const std::string& uniform, Matrix4 m);

	inline Shader* GetShader(const std::string& shader) { return shaders.find(shader)->second; };
	GLint UniformLocation(const GLchar* name) const;

	GLuint GetTexture(const std::string& texture);

	static void SetTextureRepeating(GLuint target, bool repeating);
protected:
	void AddShader(std::string name, Shader* shader);
	void BindShader(Shader*s);

	void AddTexture(std::string name, GLuint texture);

	void SetShaderLight(const Light* light);

	virtual void Resize(int x, int y);

	void StartDebugGroup(const std::string& s) {
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, (GLsizei)s.length(), s.c_str());
	}

	void EndDebugGroup() {
		glPopDebugGroup();
	}

	Window& window;

	int width;
	int height;
	bool init;

private:
	Shader* currentShader;
	std::unordered_map<std::string, Shader*> shaders;
	std::unordered_map<std::string, GLuint> textures;

	std::stack<Vector4> clearColourStack;
	std::stack<Vector4> viewportStack;

	HDC	deviceContext;
	HGLRC renderContext;
#ifdef _DEBUG
	static void CALLBACK DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
#endif
};