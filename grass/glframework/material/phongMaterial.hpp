#pragma once

#include "../core.h"
#include "../texture.h"
#include "./material.hpp"

class PhongMaterial :public Material
{
public:
	PhongMaterial();
	~PhongMaterial();

public:
	Texture* mDiffuse{ nullptr };		// ��������ɫ
	Texture* mSpecularMask{ nullptr };		// �߹��ɰ�
	float mShiness{ 1.0f };				// ��ߴ�С

};