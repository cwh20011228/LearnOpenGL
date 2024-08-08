#pragma once

#include "light.hpp"
#include "../object.hpp"

class PointLight :public Light,public Object
{
public:
	PointLight();
	~PointLight();
public:
	float mKc{ 1.0f };	// 常数项系数
	float mK1{ 1.0f };	// 一次项系数
	float mK2{ 1.0f };	// 二次项系数
};