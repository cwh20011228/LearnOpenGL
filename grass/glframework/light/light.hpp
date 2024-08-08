#pragma once 

#include "../core.h"

class Light
{
public:
	Light();
	~Light();

public:
	glm::vec3	mColor				{ 1.0f };		// 光照颜色
	float		mSpecularIntensity	{ 1.0f };		// 光照强度
	float		mInstensity			{ 1.0f };		// 整体强度

};