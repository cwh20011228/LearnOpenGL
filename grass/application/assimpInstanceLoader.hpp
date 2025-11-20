#pragma once

//TODO:模型加载类

#include "../glframework/core.h"
#include "../glframework/object.hpp"
#include "../glframework/tools/tools.hpp"
#include "../glframework/mesh/mesh.hpp"
#include "../glframework/mesh/instanceMesh.hpp"
#include "../glframework/material/phongMaterial.hpp"
#include "../glframework/material/PhongInstanceMaterial.hpp"
#include "../glframework/material/grassInstanceMaterial.hpp"
#include "../glframework/texture.h"
#include "assimpTools.hpp"

class AssimpInstanceLoader
{
public:
	static Object* load(const std::string& path,int instanceCount);

public:
	AssimpInstanceLoader();
	~AssimpInstanceLoader();

private:
	// 处理scene中的节点
	static void processNode(
		aiNode* ainode,
		Object* parent,
		const aiScene* scene,
		const std::string rootPath,
		int instanceCount);

	// 处理节点中的mesh
	static InstanceMesh* processMesh(
		aiMesh* aimesh,
		const aiScene* scene,
		const std::string rootPath,
		int instanceCount);

	// 解析模型材质贴图
	static Texture* processTexture(
		const aiMaterial* aimat,
		const aiTextureType& type,
		const aiScene* scene,
		const std::string& rootPath,
		unsigned int unit
	);

};