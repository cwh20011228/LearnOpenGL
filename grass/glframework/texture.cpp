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

	// 1. stb_image读取图片
	int channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.c_str(), &mWidth, &mHeight, &channels, STBI_rgb_alpha);

	// 2. 创建纹理对象并激活纹理单元，绑定
	GL_CALL(glGenTextures(1, &mTexture));
	GL_CALL(glActiveTexture(GL_TEXTURE0 + mUnit));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, mTexture));

	// 3. 传输纹理数据，开辟显存  (将当前level的图片信息存放在当前texture对应显存)
	GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));

	// 生成mipmap
	GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));

	// 4. 释放数据
	stbi_image_free(data);

	// 5. 设置纹理过滤方式
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	//GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));  // mipmap

	// 6. 设置纹理包裹方式 wrapping
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
}

Texture::Texture(const std::vector<std::string>& path, unsigned int unit)
{
	mUnit = unit;
	mTextureTarget = GL_TEXTURE_CUBE_MAP;

	// cubeMap不需要反转y轴
	stbi_set_flip_vertically_on_load(false);	

	// 1. 创建CubeMap对象
	GL_CALL(glGenTextures(1, &mTexture));
	GL_CALL(glActiveTexture(GL_TEXTURE0 + mUnit));
	GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, mTexture));

	// 2. 循环读取六张贴图，并且放置到cubemap的六个GPU空间
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
	// 3. 这是纹理参数
	// 设置纹理过滤方式
	GL_CALL(glTexParameteri(mTextureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL_CALL(glTexParameteri(mTextureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	//设置纹理包裹方式 wrapping
	GL_CALL(glTexParameteri(mTextureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT));	// u
	GL_CALL(glTexParameteri(mTextureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT));	// v
}

// 从内存上获取纹理
Texture::Texture(
	unsigned int unit,
	unsigned char* dataIn,
	uint32_t widthIn,
	uint32_t heightIn
)
{
	mUnit = unit;

	int channels;	//  channels_in_file：接收图像通道数的指针
	// 反转y轴
	stbi_set_flip_vertically_on_load(true);

	// 计算整张图片的大小
	//Assimp规定： 如果内嵌纹理是png或jpg压缩格式，height=0,width就代表图片大小
	uint32_t dataInSize = 0;	// 内嵌图片的大小
	if (!heightIn){
		dataInSize = widthIn;
	}
	else{
		// 如果内嵌图片不是压缩格式,height=正常高度，width=正常宽度
		// 先统一认为数据格式都是RGBA
		dataInSize = widthIn * heightIn * 4;	// 字节为单位
	}

	
	// 1. 读取图片
	unsigned char* data = stbi_load_from_memory(dataIn, dataInSize, &mWidth, &mHeight, &channels,STBI_rgb_alpha);

	// 2. 创建纹理对象并激活纹理单元
	glGenTextures(1, &mTexture);
	glActiveTexture(GL_TEXTURE0 + mUnit);
	glBindTexture(GL_TEXTURE_2D, mTexture);

	// 3.传输纹理数据，开辟显存（将当前level的图片信息存放在当前texture对应显存）
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,data);

	// 4.开启mipmap
	glGenerateMipmap(GL_TEXTURE_2D);

	// 5. 释放数据
	stbi_image_free(data);

	// 6. 设置纹理过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// 7. 设置纹理包裹方式 wrapping
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
}


// 构建空白纹理
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
	// 先切换纹理单元，再绑定texture对象
	GL_CALL(glActiveTexture(GL_TEXTURE0 + mUnit));
	GL_CALL(glBindTexture(mTextureTarget, mTexture));
}

Texture* Texture::createTexture(const std::string& path, unsigned int unit)
{
	//1. 检查是否缓存过本路径对应的纹理对象
	std::map<std::string,Texture*>::iterator iter = mTextureCache.find(path);
	if (iter != mTextureCache.end())
	{
		return iter->second;
	}
	//2. 如果本路径对应的texture没有生成过，则重新生成
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
	//1. 检查是否缓存过本路径对应的纹理对象
	std::map<std::string, Texture*>::iterator iter = mTextureCache.find(path);
	if (iter != mTextureCache.end())
	{
		return iter->second;
	}

	//2. 如果本路径对应的texture没有生成过，则重新生成
	Texture* texture = new Texture(unit,dataIn,widthIn,heightIn);
	mTextureCache[path] = texture;

	return texture;
}


// 创建颜色附件
Texture* Texture::createColorAttachment(
	unsigned int width,
	unsigned int height,
	unsigned int unit)
{
	return new Texture(width, height, unit);
}


// 创建 深度_模板测试 附件
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




