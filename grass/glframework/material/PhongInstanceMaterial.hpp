#pragma once

#include "../core.h"
#include "../texture.h"
#include "./material.hpp"

// ʹ��attribute��ʽ
class PhongInstanceMaterial :public Material
{
public:
	PhongInstanceMaterial();
	~PhongInstanceMaterial();

public:
	Texture* mDiffuse{ nullptr };		// ��������ɫ
	Texture* mSpecularMask{ nullptr };		// �߹��ɰ�
	float mShiness{ 1.0f };				// ��ߴ�С

};