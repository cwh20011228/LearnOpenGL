#pragma once

#include "../core.h"
#include "mesh.hpp"
#include <algorithm>

class InstanceMesh :public Mesh
{
public:
	InstanceMesh(Geometry* geometry, Material* material,unsigned int instanceCount);
	~InstanceMesh();

	// 更新matrixVbo
	void updataMatrices();	
	// 对matrices排序（z距离从远到近）
	void sortMatrices(glm::mat4 viewMatrix);

public:
	unsigned int			mInstanceCount{ 0 };
	// opengl默认规则，按照从0~N号进行绘制；Matrix也是按照从0~N号进行获取的；画第0号物体时，取第0号Matrix
	// 将matrices按照从远到近进行排序；每帧渲染，由于摄像机在动，每帧渲染，都要排序，排完序之后更新到vbo中
	std::vector<glm::mat4>	mInstanceMatrices{ };
	unsigned int			mMatrixVbo{ 0 };
};