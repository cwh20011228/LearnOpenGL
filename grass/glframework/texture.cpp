#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../application/stb_image.h"
#include "../wrapper/checkError.h"


std::map<std::string, Texture*> Texture::mTextureCache{};

Texture::Texture()
{

}

Texture::Texture(const std::string& path, unsigned int unit)
{
	mUnit = unit;

	// 1. stb_image��ȡͼƬ
	int channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.c_str(), &mWidth, &mHeight, &channels, STBI_rgb_alpha);

	// 2. ����������󲢼�������Ԫ����
	GL_CALL(glGenTextures(1, &mTexture));
	GL_CALL(glActiveTexture(GL_TEXTURE0 + mUnit));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, mTexture));

	// 3. �����������ݣ������Դ�  (����ǰlevel��ͼƬ��Ϣ����ڵ�ǰtexture��Ӧ�Դ�)
	GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));

	// ����mipmap
	GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));

	// 4. �ͷ�����
	stbi_image_free(data);

	// 5. ����������˷�ʽ
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	//GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));  // mipmap

	// 6. �������������ʽ wrapping
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
}

Texture::Texture(const std::vector<std::string>& path, unsigned int unit)
{
	mUnit = unit;
	mTextureTarget = GL_TEXTURE_CUBE_MAP;

	// cubeMap����Ҫ��תy��
	stbi_set_flip_vertically_on_load(false);	

	// 1. ����CubeMap����
	GL_CALL(glGenTextures(1, &mTexture));
	GL_CALL(glActiveTexture(GL_TEXTURE0 + mUnit));
	GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, mTexture));

	// 2. ѭ����ȡ������ͼ�����ҷ��õ�cubemap������GPU�ռ�
	int channels;
	int width = 0;
	int height = 0;
	unsigned char* data = nullptr;
	for (int i = 0; i < path.size(); i++)
	{
		data=stbi_load(path[i].c_str(), &width, &height, &channels, STBI_rgb_alpha);
		if (data != nullptr)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cerr << "Error:CubeMap Texture failed to load at path -" << path[i] << std::endl;
			stbi_image_free(data);
		}
	}
	// 3. �����������
	// ����������˷�ʽ
	GL_CALL(glTexParameteri(mTextureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL_CALL(glTexParameteri(mTextureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	//�������������ʽ wrapping
	GL_CALL(glTexParameteri(mTextureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT));	// u
	GL_CALL(glTexParameteri(mTextureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT));	// v
}

// ���ڴ��ϻ�ȡ����
Texture::Texture(
	unsigned int unit,
	unsigned char* dataIn,
	uint32_t widthIn,
	uint32_t heightIn
)
{
	mUnit = unit;

	int channels;	//  channels_in_file������ͼ��ͨ������ָ��
	// ��תy��
	stbi_set_flip_vertically_on_load(true);

	// ��������ͼƬ�Ĵ�С
	//Assimp�涨�� �����Ƕ������png��jpgѹ����ʽ��height=0,width�ʹ���ͼƬ��С
	uint32_t dataInSize = 0;	// ��ǶͼƬ�Ĵ�С
	if (!heightIn){
		dataInSize = widthIn;
	}
	else{
		// �����ǶͼƬ����ѹ����ʽ,height=�����߶ȣ�width=�������
		// ��ͳһ��Ϊ���ݸ�ʽ����RGBA
		dataInSize = widthIn * heightIn * 4;	// �ֽ�Ϊ��λ
	}

	
	// 1. ��ȡͼƬ
	unsigned char* data = stbi_load_from_memory(dataIn, dataInSize, &mWidth, &mHeight, &channels,STBI_rgb_alpha);

	// 2. ����������󲢼�������Ԫ
	glGenTextures(1, &mTexture);
	glActiveTexture(GL_TEXTURE0 + mUnit);
	glBindTexture(GL_TEXTURE_2D, mTexture);

	// 3.�����������ݣ������Դ棨����ǰlevel��ͼƬ��Ϣ����ڵ�ǰtexture��Ӧ�Դ棩
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,data);

	// 4.����mipmap
	glGenerateMipmap(GL_TEXTURE_2D);

	// 5. �ͷ�����
	stbi_image_free(data);

	// 6. ����������˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// 7. �������������ʽ wrapping
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
}


// �����հ�����
Texture::Texture(unsigned int width, unsigned int height, unsigned int unit)
{
	mUnit = unit;
	mWidth = width;
	mHeight = height;

	glGenTextures(1, &mTexture);
	glActiveTexture(GL_TEXTURE0+mUnit);
	glBindTexture(GL_TEXTURE_2D, mTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// u
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	// v

}

Texture::~Texture()
{
	if (mTexture != 0)
	{
		glDeleteTextures(1, &mTexture);
	}
}

void Texture::bind()
{
	// ���л�����Ԫ���ٰ�texture����
	GL_CALL(glActiveTexture(GL_TEXTURE0 + mUnit));
	GL_CALL(glBindTexture(mTextureTarget, mTexture));
}

Texture* Texture::createTexture(const std::string& path, unsigned int unit)
{
	//1. ����Ƿ񻺴����·����Ӧ���������
	std::map<std::string,Texture*>::iterator iter = mTextureCache.find(path);
	if (iter != mTextureCache.end())
	{
		return iter->second;
	}
	//2. �����·����Ӧ��textureû�����ɹ�������������
	Texture* texture = new Texture(path, unit);
	mTextureCache[path] = texture;

	return texture;
}

Texture* Texture::createTextureFromMemory(
	const std::string& path,
	unsigned int unit,
	unsigned char* dataIn,
	uint32_t widthIn,
	uint32_t heightIn)
{
	//1. ����Ƿ񻺴����·����Ӧ���������
	std::map<std::string, Texture*>::iterator iter = mTextureCache.find(path);
	if (iter != mTextureCache.end())
	{
		return iter->second;
	}

	//2. �����·����Ӧ��textureû�����ɹ�������������
	Texture* texture = new Texture(unit,dataIn,widthIn,heightIn);
	mTextureCache[path] = texture;

	return texture;
}


// ������ɫ����
Texture* Texture::createColorAttachment(
	unsigned int width,
	unsigned int height,
	unsigned int unit)
{
	return new Texture(width, height, unit);
}


// ���� ���_ģ����� ����
Texture* Texture::createDepthStencilAttachment(
	unsigned int width,
	unsigned int height,
	unsigned int unit)
{
	Texture* dsTexture = new Texture();

	unsigned int DepthStencil;
	glGenTextures(1, &DepthStencil);
	glBindTexture(GL_TEXTURE_2D, DepthStencil);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// u
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	// v

	dsTexture->mTexture = DepthStencil;
	dsTexture->mHeight  = height;
	dsTexture->mWidth   = width;
	dsTexture->mUnit    = unit;
	
	return dsTexture;
}




