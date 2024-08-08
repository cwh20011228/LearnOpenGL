#pragma once

//TODO:ģ�ͼ�����

#include "../glframework/core.h"
#include "../glframework/object.hpp"
#include "../glframework/tools/tools.hpp"
#include "../glframework/mesh/mesh.hpp"
#include "../glframework/mesh/instanceMesh.hpp"
#include "../glframework/material/phongMaterial.hpp"
#include "../glframework/material/PhongInstanceMaterial.hpp"
#include "../glframework/material/grassInstanceMaterial.hpp"
#include "../glframework/texture.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

class AssimpInstanceLoader
{
public:
	static Object* load(const std::string& path,int instanceCount);

public:
	AssimpInstanceLoader();
	~AssimpInstanceLoader();

private:
	// ����scene�еĽڵ�
	static void processNode(
		aiNode* ainode,
		Object* parent,
		const aiScene* scene,
		const std::string rootPath,
		int instanceCount);

	// ��aiMatrix4x4ת��Ϊglm::mat4
	static glm::mat4 getMat4f(aiMatrix4x4 value);

	// ����ڵ��е�mesh
	static InstanceMesh* processMesh(
		aiMesh* aimesh,
		const aiScene* scene,
		const std::string rootPath,
		int instanceCount);

	// ����ģ�Ͳ�����ͼ
	static Texture* processTexture(
		const aiMaterial* aimat,
		const aiTextureType& type,
		const aiScene* scene,
		const std::string& rootPath,
		unsigned int unit
	);

};