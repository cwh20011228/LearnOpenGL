#include "instanceUMesh.hpp"

InstanceUMesh::InstanceUMesh(
	Geometry* geometry,
	Material* material,
	unsigned int instanceCount
):Mesh(geometry, material)
{	
	mType = ObjectType::InstanceUMesh;
	mInstanceCount = instanceCount;
	mInstanceMatrices = new glm::mat4[mInstanceCount];
}


InstanceUMesh::~InstanceUMesh()
{
	if (mInstanceMatrices != nullptr)
	{
		delete[] mInstanceMatrices;
		mInstanceMatrices = nullptr;
	}
}
