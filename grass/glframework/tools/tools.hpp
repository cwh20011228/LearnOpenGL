#pragma once

#include "../core.h"

class Tools
{
public:
	// ����һ�����󣬽������е�λ�ã���ת��ϢXYZ(ŷ����)��������Ϣ
	static void decompose(glm::mat4 matrix, glm::vec3& position, glm::vec3& eularAngle, glm::vec3& scale);
};