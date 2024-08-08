#pragma once

#include "../object.hpp"
#include "../geometry.hpp"
#include "../material/material.hpp"


class Mesh:public Object
{
public:
	Mesh(Geometry* geometry,Material* material);
	~Mesh();
public:
	Geometry* mGeometry{ nullptr };
	Material* mMaterial{ nullptr };
};