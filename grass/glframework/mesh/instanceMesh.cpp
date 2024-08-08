#include "instanceMesh.hpp"

InstanceMesh::InstanceMesh(
	Geometry* geometry,
	Material* material,
	unsigned int instanceCount
):Mesh(geometry, material)
{	
	mType = ObjectType::InstanceMesh;
	mInstanceCount = instanceCount;
	mInstanceMatrices.resize(instanceCount);

	glGenBuffers(1, &mMatrixVbo);
	glBindBuffer(GL_ARRAY_BUFFER, mMatrixVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * mInstanceCount, mInstanceMatrices.data(), GL_DYNAMIC_DRAW);

	glBindVertexArray(mGeometry->getVao());
	glBindBuffer(GL_ARRAY_BUFFER, mMatrixVbo);


	for (int i = 0; i < 4; i++)
	{
		glEnableVertexAttribArray(i+4);
		glVertexAttribPointer(i+4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(float)*4*i));
		glVertexAttribDivisor(i+4, 1);
	}
	glBindVertexArray(0);	//解绑vao
}


InstanceMesh::~InstanceMesh()
{
}

// 更新matrixVbo
void InstanceMesh::updataMatrices()
{
	glBindBuffer(GL_ARRAY_BUFFER, mMatrixVbo);
	// 如果使用glBufferData进行数据更新，会导致重新分配显存空间
	// glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * mInstanceCount, mInstanceMatrices, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * mInstanceCount, mInstanceMatrices.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);		// 解绑vbo
}

// 对matrices排序（z距离从远到近）
void InstanceMesh::sortMatrices(glm::mat4 viewMatrix)
{
	std::sort(mInstanceMatrices.begin(),
		mInstanceMatrices.end(),
		[viewMatrix](const glm::mat4& a, const glm::mat4&  b) ->int
		{
			// 计算a的相机系的z(深度)(物体初始都是放在(0,0,0)点，再通过变换到其他位置)
			auto modelMatrixA = a;
			auto worldPositionA = modelMatrixA * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			auto cameraPositionA = viewMatrix * worldPositionA;

			// 计算b的相机系的z(深度)
			auto modelMatrixB = b;
			auto worldPositionB = modelMatrixB * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			auto cameraPositionB = viewMatrix * worldPositionB;

			return cameraPositionA.z < cameraPositionB.z;	// 升序（-z轴，越小说明距离越大，先绘制）
		}
	);
}