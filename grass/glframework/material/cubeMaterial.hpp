#pragma once 

#include "material.hpp"
#include "../texture.h"

class CubeMaterial:public Material
{
public:
	CubeMaterial();
	~CubeMaterial();

public:
	Texture* mDiffuse{ nullptr };
};