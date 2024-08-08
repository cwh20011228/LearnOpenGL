#pragma once

#include "../core.h"
#include "../texture.h"
#include "./material.hpp"

// 使用attribute方式
class PhongInstanceUMaterial:public Material
{
public:
	PhongInstanceUMaterial();
	~PhongInstanceUMaterial();

public:
	Texture* mDiffuse{ nullptr };		// 漫反射颜色
	Texture* mSpecularMask{ nullptr };		// 高光蒙版
	float mShiness{ 1.0f };				// 光斑大小

};