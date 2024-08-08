#pragma once

#include "../core.h"

class Tools
{
public:
	// 传入一个矩阵，解析其中的位置，旋转信息XYZ(欧拉角)，缩放信息
	static void decompose(glm::mat4 matrix, glm::vec3& position, glm::vec3& eularAngle, glm::vec3& scale);
};