#include "assimpLoader.hpp"

Object* AssimpLoader::load(const std::string& path)
{
	//�ó�ģ������Ŀ¼  ���һ��/���ֵ�λ��
	std::size_t lastIndex = path.find_last_of("//");
	std::string rootPath = path.substr(0, lastIndex + 1);


	Object* rootNode = new Object();
	Assimp::Importer importer;	// ������
	const aiScene* scene= importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals);

	// ��֤��ȡ�Ƿ���ȷ		�������ı�־λ��AI_SCENE_FLAGS_INCOMPLETE
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "Error:Model Read Failed!" << std::endl;
		return nullptr;
	}

	// �Ӹ��ڵ㿪ʼ����
	processNode(scene->mRootNode, rootNode, scene, rootPath);

	return rootNode;
}


void AssimpLoader::processNode(
	aiNode* ainode, 
	Object* parent, 
	const aiScene* scene,
	const std::string rootPath)
{
	Object* node = new Object();
	parent->addChild(node);

	glm::mat4 localMatrix = getMat4f(ainode->mTransformation);

	// λ��
	glm::vec3 position;
	// ��ת(ŷ����)
	glm::vec3 eulerAbgle;
	// ����
	glm::vec3 scale;

	Tools::decompose(localMatrix, position, eulerAbgle, scale);

	node->setPosition(position);
	node->setAngleX(eulerAbgle.x);
	node->setAngleY(eulerAbgle.y);
	node->setAngleZ(eulerAbgle.z);
	node->setScale(scale);

	//�����û��mesh�������н���
	for (int i = 0; i < ainode->mNumMeshes; i++)
	{
		int meshId = ainode->mMeshes[i];
		aiMesh* aimesh = scene->mMeshes[meshId];
		auto mesh = processMesh(aimesh,scene, rootPath);
		node->addChild(mesh);
	}

	for (int i = 0; i < ainode->mNumChildren; i++)
	{
		processNode(ainode->mChildren[i],node,scene, rootPath);
	}

}

Mesh* AssimpLoader::processMesh(
	aiMesh* aimesh, 
	const aiScene* scene,
	const std::string rootPath)
{
	std::vector<float> positions;
	std::vector<float> normals;
	std::vector<float> uvs;
	std::vector<unsigned int> indices;

	for (int i = 0; i < aimesh->mNumVertices; i++)
	{
		// ��i�������λ��
		positions.push_back(aimesh->mVertices[i].x);
		positions.push_back(aimesh->mVertices[i].y);
		positions.push_back(aimesh->mVertices[i].z);

		// ��i������ķ���
		normals.push_back(aimesh->mNormals[i].x);
		normals.push_back(aimesh->mNormals[i].y);
		normals.push_back(aimesh->mNormals[i].z);

		// ��i�������uv	
		// mTextureCoords[0] ��0��uv����ɫ��ͼuv��,uv�����кü��ף���ɫ/������ͼuv �ȵ�
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
	
	// ����mesh�е�����ֵ
	for (int i = 0; i < aimesh->mNumFaces; i++)
	{
		aiFace face = aimesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
		
	}

	Geometry* geometry = new Geometry(positions, normals, uvs, indices);
	PhongMaterial* material = new PhongMaterial();


	// �в���
	if (aimesh->mMaterialIndex >= 0)
	{
		Texture* texture{ nullptr };
		aiMaterial* aiMat = scene->mMaterials[aimesh->mMaterialIndex];

		// ������û�и߹��ɰ�ʱ����
		//material->mSpecularMask= new Texture("../assets/textures/defaultTexture.jpg", 1);

		//TODO: 1. ��ȡdiffuse��ͼ
		texture = processTexture(aiMat, aiTextureType_DIFFUSE, scene, rootPath, 0);
		if (texture == nullptr)
		{
			texture = new Texture("../assets/textures/defaultTexture.jpg", 0);
		}
		material->mDiffuse = texture;

		//TODO: 2. ��ȡspecular��ͼ���������и߹��ɰ�ʱ���ã�
		Texture* specularMask = processTexture(aiMat, aiTextureType_SPECULAR, scene, rootPath, 1);
		if (specularMask == nullptr)
		{
			specularMask = new Texture("../assets/textures/defaultTexture.jpg", 1);
		}
		//specularMask->setUnit(1);
		material->mSpecularMask = specularMask;
	}
	else{
		// û�в���,���ֶ���һ������ͼƬ
		material->mDiffuse = new Texture("../assets/textures/defaultTexture.jpg", 0);
	}


	return new Mesh(geometry, material);
}

glm::mat4 AssimpLoader::getMat4f(aiMatrix4x4 value)
{
	glm::mat4 to(
		value.a1, value.a2, value.a3, value.a4,
		value.b1, value.b2, value.b3, value.b4,
		value.c1, value.c2, value.c3, value.c4,
		value.d1, value.d2, value.d3, value.d4
	);
	
	return to;
}


AssimpLoader::AssimpLoader()
{

}

AssimpLoader::~AssimpLoader()
{

}

Texture* AssimpLoader::processTexture(
	const aiMaterial* aimat,
	const aiTextureType& type,
	const aiScene* scene,
	const std::string& rootPath,
	unsigned int unit
)
{
	Texture* texture{ nullptr };
	// ��ȡͼƬ��ȡ·��
	aiString aipath;
	// aiTextureType_DIFFUSE ��������ͼ  0 ��0������ͼƬ��Ϣ(��������ͼ�����ж��������ȡ��0��)
	aimat->Get(AI_MATKEY_TEXTURE(type, 0), aipath);
	if (!aipath.length)
	{
		return nullptr;
	}


	/* �����ȡ����һ�����·���� aipath = "test.fbm\\box.png"
		Ӧ����   "assets/fbx/test"  +  "test.fbm/box.png"����ģ����Ե�·����*/


		//�ж��Ƿ�Ƕ��fbx��ͼƬ
	const aiTexture* aitexture = scene->GetEmbeddedTexture(aipath.C_Str());
	if (aitexture) {
		// ����ͼƬ����Ƕ��(���ڴ���)
		unsigned char* dataIn = reinterpret_cast<unsigned char*>(aitexture->pcData);
		uint32_t widthIn = aitexture->mWidth;		// ͨ������£�png,jpg��ѹ��ͼƬ����,mWidth�ʹ�������ͼƬ��С
		uint32_t heightIn = aitexture->mHeight;

		texture = Texture::createTextureFromMemory(aipath.C_Str(), unit, dataIn, widthIn, heightIn);
	}
	else {
		// ����ͼƬ��Ӳ����
		std::string fullpath = rootPath + aipath.C_Str();
		texture = Texture::createTexture(fullpath, unit);
	}
	return texture;
}



