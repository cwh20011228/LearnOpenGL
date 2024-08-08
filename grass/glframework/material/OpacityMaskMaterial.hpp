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
	Texture* mDiffuse{ nullptr };		// ��������ɫ
	Texture* mOpacityMask{ nullptr };		// opacityMask
	float mShiness{ 1.0f };				// ��ߴ�С

};