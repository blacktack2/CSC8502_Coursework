#pragma once
#include "Mesh.h"
#include "OGLRenderer.h"

class ProceduralHeightMap {
public:
	ProceduralHeightMap(OGLRenderer& renderer, Mesh* quad, int x, int z);
	~ProceduralHeightMap();

	void Generate();
	void DeGenerate();

	inline GLuint GetHeightTex() { return heightTex; }

	GLsizei resolutionX = 1;
	GLsizei resolutionZ = 1;
	float worldX = 1.0f;
	float worldZ = 1.0f;
protected:
	OGLRenderer& renderer;
	Mesh* quad;

	int offsetX;
	int offsetZ;

	GLuint generationFBO = 0;
	GLuint heightTex = 0;
};
