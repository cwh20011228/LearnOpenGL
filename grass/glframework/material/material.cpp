#include "./material.hpp"

Material::Material()
{

}

Material::~Material()
{

}

void Material::setDepthFunc(GLenum func)
{
	mDepthFunc = func;
}

// 设置斜率因子
void Material::setFactor(float factor)
{
	mFactor = factor;
}

// 设置偏移因子
void Material::setUnit(float unit)
{
	mUnit = unit;
}

void Material::setStencilTest(
	bool			StencilTest,
	unsigned int	SFail,				// 模板测试失败了怎么办
	unsigned int	ZFail,				// 模板测试通过但深度测试失败了怎么办
	unsigned int	Pass,				// 模板测试和深度测试都通过了怎么办
	unsigned int	StencilMask,		// 控制模板写入
	unsigned int	StencilFunc,
	unsigned int	StencilRef,		// 参考数值
	unsigned int	StencilFuncMask
)
{
	mStencilTest = StencilTest;

	mSFail = SFail;
	mZFail = ZFail;
	mPass = Pass;

	mStencilMask = StencilMask;

	mStencilFunc = StencilFunc;
	mStencilRef = StencilRef;
	mStencilFuncMask = StencilFuncMask;

}

// 设置颜色混合
void Material::setBlend(
	bool blend,
	unsigned int sfactor,
	unsigned int dfactor,
	float opacity)
{
	mBlend = blend;
	mSFactor = sfactor;
	mDFactor = dfactor;
	mOpacity = opacity;
}

// 设置面剔除参数
void Material::setFaceCull(
	bool faceculling,
	unsigned int frontFace,
	unsigned int cullface)
{
	mFaceCulling = faceculling;
	mFrontFace = frontFace;
	mCullFace = cullface;
}