#pragma once

#include "../core.h"
#include "../texture.h"
#include "./material.hpp"

class PhongEnvMaterial:public Material
{
public:
	PhongEnvMaterial();
	~PhongEnvMaterial();

public:
	Texture* mDiffuse{ nullptr };		// 漫反射颜色
	Texture* mSpecularMask{ nullptr };	// 高光蒙版
	float mShiness{ 1.0f };				// 光斑大小
	Texture* mEnv{ nullptr };			// 环境颜色

};