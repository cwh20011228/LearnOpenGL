#pragma once 

#include "light.hpp"
#include "../object.hpp"

class SpotLight :public Light,public Object
{
public:
	SpotLight();
	~SpotLight();

public:
	glm::vec3 mTargetDirection{ -1.0f,0.0f,0.0f };	// �۹�Ƴ���ķ���
	float mInnerAngle{ 0.0f };			// �۹��������Χ��һ��Ƕȣ��ڱ߽��߽Ƕȣ�
	float mOuterAngle{ 0.0f };			// ��������ĽǶȣ���߽��߽Ƕȣ�
};