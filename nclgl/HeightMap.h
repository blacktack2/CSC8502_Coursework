#pragma once
#include "Mesh.h"

#include <string>

class HeightMap : public Mesh {
public:
	explicit HeightMap(const std::string& name);
	~HeightMap() = default;

	inline Vector3 GetSize() const { return size; }
protected:
	Vector3 size;
};

