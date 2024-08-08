#pragma once

#include "../core.h"
#include "../texture.h"
#include "./material.hpp"

class OpacityMaskMaterial :public Material
{
public:
	OpacityMaskMaterial();
	~OpacityMaskMaterial();

public:
	Texture* mDiffuse{ nullptr };		// 漫反射颜色
	Texture* mOpacityMask{ nullptr };		// opacityMask
	float mShiness{ 1.0f };				// 光斑大小

};