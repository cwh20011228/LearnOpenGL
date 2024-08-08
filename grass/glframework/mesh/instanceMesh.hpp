#pragma once

#include "../core.h"
#include "mesh.hpp"
#include <algorithm>

class InstanceMesh :public Mesh
{
public:
	InstanceMesh(Geometry* geometry, Material* material,unsigned int instanceCount);
	~InstanceMesh();

	// ����matrixVbo
	void updataMatrices();	
	// ��matrices����z�����Զ������
	void sortMatrices(glm::mat4 viewMatrix);

public:
	unsigned int			mInstanceCount{ 0 };
	// openglĬ�Ϲ��򣬰��մ�0~N�Ž��л��ƣ�MatrixҲ�ǰ��մ�0~N�Ž��л�ȡ�ģ�����0������ʱ��ȡ��0��Matrix
	// ��matrices���մ�Զ������������ÿ֡��Ⱦ������������ڶ���ÿ֡��Ⱦ����Ҫ����������֮����µ�vbo��
	std::vector<glm::mat4>	mInstanceMatrices{ };
	unsigned int			mMatrixVbo{ 0 };
};