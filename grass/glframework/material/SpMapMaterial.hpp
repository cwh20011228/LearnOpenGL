#pragma once 

#include "material.hpp"
#include "../texture.h"

class SpMapMaterial:public Material
{
public:
	SpMapMaterial();
	~SpMapMaterial();

public:
	Texture* mDiffuse{ nullptr };
};