#pragma once

#include "../glframework/core.h"

class AssimpTools
{
public:
	static glm::vec3 getGLMVec3(aiVector3D _inVec)
	{
		return glm::vec3(_inVec.x, _inVec.y, _inVec.z);
	}

	static glm::quat getGLMQuat(aiQuaternion _quat)
	{
		return glm::quat(_quat.w, _quat.x, _quat.y, _quat.z);
	}

	static glm::mat4 getGLMat4(aiMatrix4x4 value)
	{
		glm::mat4 to(
			value.a1, value.a2, value.a3, value.a4,
			value.b1, value.b2, value.b3, value.b4,
			value.c1, value.c2, value.c3, value.c4,
			value.d1, value.d2, value.d3, value.d4
		);

		return to;
	}

};