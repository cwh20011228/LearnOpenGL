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
	glBindVertexArray(0);	//���vao
}


InstanceMesh::~InstanceMesh()
{
}

// ����matrixVbo
void InstanceMesh::updataMatrices()
{
	glBindBuffer(GL_ARRAY_BUFFER, mMatrixVbo);
	// ���ʹ��glBufferData�������ݸ��£��ᵼ�����·����Դ�ռ�
	// glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * mInstanceCount, mInstanceMatrices, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * mInstanceCount, mInstanceMatrices.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);		// ���vbo
}

// ��matrices����z�����Զ������
void InstanceMesh::sortMatrices(glm::mat4 viewMatrix)
{
	std::sort(mInstanceMatrices.begin(),
		mInstanceMatrices.end(),
		[viewMatrix](const glm::mat4& a, const glm::mat4&  b) ->int
		{
			// ����a�����ϵ��z(���)(�����ʼ���Ƿ���(0,0,0)�㣬��ͨ���任������λ��)
			auto modelMatrixA = a;
			auto worldPositionA = modelMatrixA * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			auto cameraPositionA = viewMatrix * worldPositionA;

			// ����b�����ϵ��z(���)
			auto modelMatrixB = b;
			auto worldPositionB = modelMatrixB * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			auto cameraPositionB = viewMatrix * worldPositionB;

			return cameraPositionA.z < cameraPositionB.z;	// ����-z�ᣬԽС˵������Խ���Ȼ��ƣ�
		}
	);
}