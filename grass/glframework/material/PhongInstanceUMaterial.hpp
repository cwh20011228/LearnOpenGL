#pragma once

#include "../core.h"
#include "../texture.h"
#include "./material.hpp"

// ʹ��attribute��ʽ
class PhongInstanceUMaterial:public Material
{
public:
	PhongInstanceUMaterial();
	~PhongInstanceUMaterial();

public:
	Texture* mDiffuse{ nullptr };		// ��������ɫ
	Texture* mSpecularMask{ nullptr };		// �߹��ɰ�
	float mShiness{ 1.0f };				// ��ߴ�С

};