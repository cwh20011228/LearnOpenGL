#include "assimpInstanceLoader.hpp"

Object* AssimpInstanceLoader::load(const std::string& path,int instanceCount)
{
	//拿出模型所在目录  最后一个/出现的位置
	std::size_t lastIndex = path.find_last_of("//");
	std::string rootPath = path.substr(0, lastIndex + 1);


	Object* rootNode = new Object();
	Assimp::Importer importer;	// 导入器
	const aiScene* scene= importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals);

	// 验证读取是否正确		不完整的标志位：AI_SCENE_FLAGS_INCOMPLETE
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "Error:Model Read Failed!" << std::endl;
		return nullptr;
	}

	// 从根节点开始处理
	processNode(scene->mRootNode, rootNode, scene, rootPath,instanceCount);

	return rootNode;
}


void AssimpInstanceLoader::processNode(
	aiNode* ainode, 
	Object* parent, 
	const aiScene* scene,
	const std::string rootPath,
	int instanceCount)
{
	Object* node = new Object();
	parent->addChild(node);

	glm::mat4 localMatrix = getMat4f(ainode->mTransformation);

	// 位置
	glm::vec3 position;
	// 旋转(欧拉角)
	glm::vec3 eulerAbgle;
	// 缩放
	glm::vec3 scale;

	Tools::decompose(localMatrix, position, eulerAbgle, scale);

	node->setPosition(position);
	node->setAngleX(eulerAbgle.x);
	node->setAngleY(eulerAbgle.y);
	node->setAngleZ(eulerAbgle.z);
	node->setScale(scale);

	//检查有没有mesh，并进行解析
	for (int i = 0; i < ainode->mNumMeshes; i++)
	{
		int meshId = ainode->mMeshes[i];
		aiMesh* aimesh = scene->mMeshes[meshId];
		auto mesh = processMesh(aimesh,scene, rootPath, instanceCount);
		node->addChild(mesh);
	}

	for (int i = 0; i < ainode->mNumChildren; i++)
	{
		processNode(ainode->mChildren[i],node,scene, rootPath, instanceCount);
	}

}

InstanceMesh* AssimpInstanceLoader::processMesh(
	aiMesh* aimesh, 
	const aiScene* scene,
	const std::string rootPath,
	int instanceCount)
{
	std::vector<float> positions;
	std::vector<float> normals;
	std::vector<float> uvs;
	std::vector<unsigned int> indices;
	std::vector<float> colors;

	for (int i = 0; i < aimesh->mNumVertices; i++)
	{
		// 第i个顶点的位置
		positions.push_back(aimesh->mVertices[i].x);
		positions.push_back(aimesh->mVertices[i].y);
		positions.push_back(aimesh->mVertices[i].z);

		// 第i个顶点的法线
		normals.push_back(aimesh->mNormals[i].x);
		normals.push_back(aimesh->mNormals[i].y);
		normals.push_back(aimesh->mNormals[i].z);

		// 第i个顶点的颜色
		// 每一个顶点可能有n套颜色，默认读取第0套
		if (aimesh->HasVertexColors(0))
		{
			colors.push_back(aimesh->mColors[0][i].r);
			colors.push_back(aimesh->mColors[0][i].g);
			colors.push_back(aimesh->mColors[0][i].b);
		}
		
		// 第i个顶点的uv	
		// mTextureCoords[0] 第0套uv（颜色贴图uv）,uv可能有好几套，颜色/光照贴图uv 等等
		if (aimesh->mTextureCoords[0] != nullptr)
		{
			uvs.push_back(aimesh->mTextureCoords[0][i].x);
			uvs.push_back(aimesh->mTextureCoords[0][i].y);
		}
		else
		{
			uvs.push_back(0.0f);
			uvs.push_back(0.0f);
		}
		
	}
	
	// 解析mesh中的索引值
	for (int i = 0; i < aimesh->mNumFaces; i++)
	{
		aiFace face = aimesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
		
	}

	Geometry* geometry = new Geometry(positions, normals, uvs,colors,indices);
	auto material = new PhongInstanceMaterial();


	// 有材质
	if (aimesh->mMaterialIndex >= 0)
	{
		Texture* texture{ nullptr };
		aiMaterial* aiMat = scene->mMaterials[aimesh->mMaterialIndex];

		// 材质中没有高光蒙版时启用
		//material->mSpecularMask= new Texture("../assets/textures/defaultTexture.jpg", 1);

		//TODO: 1. 读取diffuse贴图
		texture = processTexture(aiMat, aiTextureType_DIFFUSE, scene, rootPath, 0);
		if (texture == nullptr)
		{
			texture = new Texture("assets/textures/defaultTexture.jpg", 0);
		}
		material->mDiffuse = texture;

		//TODO: 2. 读取specular贴图（材质中有高光蒙版时启用）
		Texture* specularMask = processTexture(aiMat, aiTextureType_SPECULAR, scene, rootPath, 1);
		if (specularMask == nullptr)
		{
			specularMask = new Texture("assets/textures/defaultTexture.jpg", 1);
		}
		//specularMask->setUnit(1);
		material->mSpecularMask = specularMask;
	}
	else{
		// 没有材质,就手动给一个材质图片
		material->mDiffuse = new Texture("assets/textures/defaultTexture.jpg", 0);
	}


	return new InstanceMesh(geometry, material,instanceCount);
}

glm::mat4 AssimpInstanceLoader::getMat4f(aiMatrix4x4 value)
{
	glm::mat4 to(
		value.a1, value.a2, value.a3, value.a4,
		value.b1, value.b2, value.b3, value.b4,
		value.c1, value.c2, value.c3, value.c4,
		value.d1, value.d2, value.d3, value.d4
	);
	
	return to;
}


AssimpInstanceLoader::AssimpInstanceLoader()
{

}

AssimpInstanceLoader::~AssimpInstanceLoader()
{

}

Texture* AssimpInstanceLoader::processTexture(
	const aiMaterial* aimat,
	const aiTextureType& type,
	const aiScene* scene,
	const std::string& rootPath,
	unsigned int unit
)
{
	Texture* texture{ nullptr };
	// 获取图片读取路径
	aiString aipath;
	// aiTextureType_DIFFUSE 漫反射贴图  0 第0张纹理图片信息(漫反射贴图可能有多个，这里取第0个)
	aimat->Get(AI_MATKEY_TEXTURE(type, 0), aipath);
	if (!aipath.length)
	{
		return nullptr;
	}


	/* 上面读取后（是一个相对路径） aipath = "test.fbm\\box.png"
		应该是   "assets/fbx/test"  +  "test.fbm/box.png"（跟模型相对的路径）*/


		//判断是否嵌入fbx的图片
	const aiTexture* aitexture = scene->GetEmbeddedTexture(aipath.C_Str());
	if (aitexture) {
		// 纹理图片是内嵌的(在内存上)
		unsigned char* dataIn = reinterpret_cast<unsigned char*>(aitexture->pcData);
		uint32_t widthIn = aitexture->mWidth;		// 通常情况下（png,jpg（压缩图片））,mWidth就代表整张图片大小
		uint32_t heightIn = aitexture->mHeight;

		texture = Texture::createTextureFromMemory(aipath.C_Str(), unit, dataIn, widthIn, heightIn);
	}
	else {
		// 纹理图片在硬盘上
		std::string fullpath = rootPath + aipath.C_Str();
		texture = Texture::createTexture(fullpath, unit);
	}
	return texture;
}



