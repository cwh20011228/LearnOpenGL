#pragma once 

#include "../core.h"

class Light
{
public:
	Light();
	~Light();

public:
	glm::vec3	mColor				{ 1.0f };		// ������ɫ
	float		mSpecularIntensity	{ 1.0f };		// ����ǿ��
	float		mInstensity			{ 1.0f };		// ����ǿ��

};