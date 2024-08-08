#pragma once 

#include "material.hpp"
#include "../texture.h"

class ScreenMaterial:public Material
{
public:
	ScreenMaterial();
	~ScreenMaterial();

public:
	Texture* mScreenTexture{ nullptr };
};