#include "tools.hpp"

// ����һ�����󣬽������е�λ�ã���ת��ϢXYZ(ŷ����)��������Ϣ
void Tools::decompose(glm::mat4 matrix,
					glm::vec3& position, 
					glm::vec3& eularAngle, 
					glm::vec3& scale)
{
	// ��Ԫ����������ʾ��ת�任
	glm::quat quaternion;
	glm::vec3 skew;		// б��
	glm::vec4 perspective;
	// �⹹ģ�ͱ任����
	glm::decompose(matrix, scale, quaternion, position, skew, perspective);
	// ��Ԫ��-->ŷ����
	glm::mat4 rotation = glm::toMat4(quaternion);
	glm::extractEulerAngleXYZ(rotation, eularAngle.x, eularAngle.y, eularAngle.z);

	// �⹹�����ĽǶ��ǻ��ȣ���Ҫת��Ϊ�Ƕ�
	eularAngle.x = glm::degrees(eularAngle.x);
	eularAngle.y = glm::degrees(eularAngle.y);
	eularAngle.z = glm::degrees(eularAngle.z);
}