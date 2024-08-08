#pragma once

#include "core.h"
#include <string>

class Texture
{
public:
	Texture();
	// ��Ӳ�̻�ȡ����
	Texture(const std::string& path,unsigned int unit);

	// ���ڴ��ϻ�ȡ����
	Texture(
		unsigned int unit,
		unsigned char* dataIn,
		uint32_t widthIn,
		uint32_t heightIn
	);

	// �����հ�����
	Texture(unsigned int width, unsigned int height, unsigned int unit);

	// path�е�˳���������º�ǰ��+x -x +y -y +z -z��
	Texture(const std::vector<std::string>& path, unsigned int unit);

	~Texture();

	int getWidth() const { return mWidth; }
	int getHeight() const { return mHeight; }
	GLuint getTexture() const { return mTexture; }

	void setUnit(unsigned int unit) { mUnit = unit; }
	void bind();

public:
	// ��Ӳ�̶�ȡ�ļ�
	static Texture* createTexture(const std::string& path, unsigned int unit);

	// ���ڴ��ȡ�ļ�
	static Texture* createTextureFromMemory(
		const std::string& path, 
		unsigned int unit,
		unsigned char* dataIn,
		uint32_t widthIn,
		uint32_t heightIn);

	// ������ɫ����
	static Texture* createColorAttachment(
		unsigned int width, 
		unsigned int height, 
		unsigned int unit);

	// ���� ���_ģ����� ����
	static Texture* createDepthStencilAttachment(
		unsigned int width,
		unsigned int height,
		unsigned int unit);

private:
	GLuint mTexture{0};
	int mWidth{ 0 };
	int mHeight{ 0 };
	unsigned int mUnit{ 0 };
	unsigned int mTextureTarget{ GL_TEXTURE_2D };
	static std::map<std::string, Texture*> mTextureCache;

};