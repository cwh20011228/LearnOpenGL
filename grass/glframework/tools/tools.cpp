#include "tools.hpp"

// 传入一个矩阵，解析其中的位置，旋转信息XYZ(欧拉角)，缩放信息
void Tools::decompose(glm::mat4 matrix,
					glm::vec3& position, 
					glm::vec3& eularAngle, 
					glm::vec3& scale)
{
	// 四元数，用来表示旋转变换
	glm::quat quaternion;
	glm::vec3 skew;		// 斜切
	glm::vec4 perspective;
	// 解构模型变换矩阵
	glm::decompose(matrix, scale, quaternion, position, skew, perspective);
	// 四元数-->欧拉角
	glm::mat4 rotation = glm::toMat4(quaternion);
	glm::extractEulerAngleXYZ(rotation, eularAngle.x, eularAngle.y, eularAngle.z);

	// 解构出来的角度是弧度，需要转换为角度
	eularAngle.x = glm::degrees(eularAngle.x);
	eularAngle.y = glm::degrees(eularAngle.y);
	eularAngle.z = glm::degrees(eularAngle.z);
}