#pragma once

#include "../core.h"

// 使用c++的枚举类型
enum class MaterialType
{
	PhongMaterial,
	WhiteMaterial,
	DepthMaterial,
	OpacityMaskMaterial,
	ScreenMaterial,
	CubeMaterial,			// 使用立方体贴图
	PhongEnvMaterial,		// 将立方体贴图作为环境光的输入
	SpMapMaterial,			// 使用球形投影贴图
	PhongInstanceMaterial,	// 使用attribute
	PhongInstanceUMaterial,	// 使用uniform方式
	GrassInstanceMaterial
};


class Material
{
public:
	Material();
	~Material();

public:
	// 设置深度检测使用什么函数
	void setDepthFunc(GLenum func);
	// 设置斜率因子
	void setFactor(float factor);
	// 设置偏移因子
	void setUnit(float unit);
	// 设置模板测试
	void setStencilTest(
		bool			StencilTest,
		unsigned int	SFail,				// 模板测试失败了怎么办
		unsigned int	ZFail,				// 模板测试通过但深度测试失败了怎么办
		unsigned int	Pass,				// 模板测试和深度测试都通过了怎么办
		unsigned int	StencilMask,		// 控制模板写入
		unsigned int	StencilFunc,
		unsigned int	StencilRef,			// 参考数值
		unsigned int	StencilFuncMask
	);
	// 设置颜色混合
	void setBlend(
		bool blend, 
		unsigned int sfactor, 
		unsigned int dfactor,
		float opacity);
	// 设置面剔除参数
	void setFaceCull(
		bool faceculling, 
		unsigned int frontFace, 
		unsigned int cullface);

public:
	MaterialType mType;
	// 深度检测相关
	bool	mDepthTest{ true };		// 深度检测
	GLenum	mDepthFunc{ GL_LEQUAL };	// 深度检测用什么方式(小于等于)
	bool	mDepthWrite{ true };	// 深度写入

	// polygonOffset相关（深度偏移）
	bool			mPolygonOffset{ false };
	unsigned int	mPolygonOffsetType{ GL_POLYGON_OFFSET_FILL };
	float			mFactor{ 0.0f };	// 深度斜率因子
	float			mUnit{ 0.0f };		// 偏移因子

	// stencil相关（模板测试）
	bool			mStencilTest{ false };

	unsigned int	mSFail{ GL_KEEP };		// 模板测试失败了怎么办
	unsigned int	mZFail{ GL_KEEP };		// 模板测试通过但深度测试失败了怎么办
	unsigned int	mPass{ GL_KEEP };		// 模板测试和深度测试都通过了怎么办

	unsigned int	mStencilMask{ 0xFF };	// 控制模板写入

	unsigned int	mStencilFunc{ GL_ALWAYS };	
	unsigned int	mStencilRef{ 0 };			// 参考数值
	unsigned int	mStencilFuncMask{ 0xFF };

	// 颜色混合
	bool			mBlend{ false };				
	unsigned int	mSFactor{ GL_SRC_ALPHA };				// 源颜色使用什么值作为透明度
	unsigned int	mDFactor{ GL_ONE_MINUS_SRC_ALPHA };		// 目标颜色使用什么值作为透明度
	float			mOpacity{ 1.0f };						// 总体透明度

	// 面剔除
	bool		mFaceCulling{ false };
	unsigned int mFrontFace{ GL_CCW };		// 正面的定义（默认逆时针为正面）
	unsigned int mCullFace{ GL_BACK };		// 剔除的面

};	