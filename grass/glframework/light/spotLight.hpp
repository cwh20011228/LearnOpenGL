#pragma once 

#include "light.hpp"
#include "../object.hpp"

class SpotLight :public Light,public Object
{
public:
	SpotLight();
	~SpotLight();

public:
	glm::vec3 mTargetDirection{ -1.0f,0.0f,0.0f };	// 聚光灯朝向的方向
	float mInnerAngle{ 0.0f };			// 聚光灯照亮范围的一半角度（内边界线角度）
	float mOuterAngle{ 0.0f };			// 过渡区域的角度（外边界线角度）
};