#pragma once

#include "light.hpp"
#include "../object.hpp"

class PointLight :public Light,public Object
{
public:
	PointLight();
	~PointLight();
public:
	float mKc{ 1.0f };	// ������ϵ��
	float mK1{ 1.0f };	// һ����ϵ��
	float mK2{ 1.0f };	// ������ϵ��
};