#include "HeightMap.h"

HeightMap::HeightMap(const std::string& name) {
	int iWidth, iHeight, iChans;

	unsigned char* data = SOIL_load_image(name.c_str(), &iWidth, &iHeight, &iChans, 1);
	if (!data) {
		std::cout << "Unable to load heightmap file!\n";
		return;
	}

	numVertices = iWidth * iHeight;
	numIndices = (iWidth - 1) * (iHeight - 1) * 6;
	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];

	const Vector3 vertexScale = Vector3(16.0f, 1.0f, 16.0f);
	const Vector2 textureScale = Vector2(1.0f / 16.0f, 1.0f / 16.0f);
	for (int z = 0; z < iHeight; z++) {
		for (int x = 0; x < iWidth; x++) {
			int offset = (z * iWidth) + x;
			vertices[offset] = Vector3(x, data[offset], z) * vertexScale;
			textureCoords[offset] = Vector2(x, z) * textureScale;
		}
	}
	SOIL_free_image_data(data);

	int i = 0;
	for (int z = 0; z < iHeight - 1; z++) {
		for (int x = 0; x < iWidth - 1; x++) {
			int a = (z * iWidth) + x;
			int b = (z * iWidth) + x + 1;
			int c = ((z + 1) * iWidth) + x + 1;
			int d = ((z + 1) * iWidth) + x;

			indices[i++] = a;
			indices[i++] = c;
			indices[i++] = b;

			indices[i++] = c;
			indices[i++] = a;
			indices[i++] = d;
		}
	}

	GenerateNormals();
	GenerateTangents();
	BufferData();

	size = vertexScale * Vector3(iWidth - 1, 255.0f, iHeight - 1);
}
