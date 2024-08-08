#pragma once

#include "../core.h"
#include "mesh.hpp"

class InstanceUMesh :public Mesh
{
public:
	InstanceUMesh(Geometry* geometry, Material* material,unsigned int instanceCount);
	~InstanceUMesh();

public:
	unsigned int mInstanceCount{ 0 };
	glm::mat4* mInstanceMatrices{ nullptr };
};