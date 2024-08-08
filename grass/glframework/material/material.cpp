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

// ����б������
void Material::setFactor(float factor)
{
	mFactor = factor;
}

// ����ƫ������
void Material::setUnit(float unit)
{
	mUnit = unit;
}

void Material::setStencilTest(
	bool			StencilTest,
	unsigned int	SFail,				// ģ�����ʧ������ô��
	unsigned int	ZFail,				// ģ�����ͨ������Ȳ���ʧ������ô��
	unsigned int	Pass,				// ģ����Ժ���Ȳ��Զ�ͨ������ô��
	unsigned int	StencilMask,		// ����ģ��д��
	unsigned int	StencilFunc,
	unsigned int	StencilRef,		// �ο���ֵ
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

// ������ɫ���
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

// �������޳�����
void Material::setFaceCull(
	bool faceculling,
	unsigned int frontFace,
	unsigned int cullface)
{
	mFaceCulling = faceculling;
	mFrontFace = frontFace;
	mCullFace = cullface;
}