#pragma once

#include "core.h"
#include <string>

class Texture
{
public:
	Texture();
	// 从硬盘获取纹理
	Texture(const std::string& path,unsigned int unit);

	// 从内存上获取纹理
	Texture(
		unsigned int unit,
		unsigned char* dataIn,
		uint32_t widthIn,
		uint32_t heightIn
	);

	// 构建空白纹理
	Texture(unsigned int width, unsigned int height, unsigned int unit);

	// path中的顺序：右左上下后前（+x -x +y -y +z -z）
	Texture(const std::vector<std::string>& path, unsigned int unit);

	~Texture();

	int getWidth() const { return mWidth; }
	int getHeight() const { return mHeight; }
	GLuint getTexture() const { return mTexture; }

	void setUnit(unsigned int unit) { mUnit = unit; }
	void bind();

public:
	// 从硬盘读取文件
	static Texture* createTexture(const std::string& path, unsigned int unit);

	// 从内存读取文件
	static Texture* createTextureFromMemory(
		const std::string& path, 
		unsigned int unit,
		unsigned char* dataIn,
		uint32_t widthIn,
		uint32_t heightIn);

	// 创建颜色附件
	static Texture* createColorAttachment(
		unsigned int width, 
		unsigned int height, 
		unsigned int unit);

	// 创建 深度_模板测试 附件
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