#pragma once

#include "../core.h"

// ʹ��c++��ö������
enum class MaterialType
{
	PhongMaterial,
	WhiteMaterial,
	DepthMaterial,
	OpacityMaskMaterial,
	ScreenMaterial,
	CubeMaterial,			// ʹ����������ͼ
	PhongEnvMaterial,		// ����������ͼ��Ϊ�����������
	SpMapMaterial,			// ʹ������ͶӰ��ͼ
	PhongInstanceMaterial,	// ʹ��attribute
	PhongInstanceUMaterial,	// ʹ��uniform��ʽ
	GrassInstanceMaterial
};


class Material
{
public:
	Material();
	~Material();

public:
	// ������ȼ��ʹ��ʲô����
	void setDepthFunc(GLenum func);
	// ����б������
	void setFactor(float factor);
	// ����ƫ������
	void setUnit(float unit);
	// ����ģ�����
	void setStencilTest(
		bool			StencilTest,
		unsigned int	SFail,				// ģ�����ʧ������ô��
		unsigned int	ZFail,				// ģ�����ͨ������Ȳ���ʧ������ô��
		unsigned int	Pass,				// ģ����Ժ���Ȳ��Զ�ͨ������ô��
		unsigned int	StencilMask,		// ����ģ��д��
		unsigned int	StencilFunc,
		unsigned int	StencilRef,			// �ο���ֵ
		unsigned int	StencilFuncMask
	);
	// ������ɫ���
	void setBlend(
		bool blend, 
		unsigned int sfactor, 
		unsigned int dfactor,
		float opacity);
	// �������޳�����
	void setFaceCull(
		bool faceculling, 
		unsigned int frontFace, 
		unsigned int cullface);

public:
	MaterialType mType;
	// ��ȼ�����
	bool	mDepthTest{ true };		// ��ȼ��
	GLenum	mDepthFunc{ GL_LEQUAL };	// ��ȼ����ʲô��ʽ(С�ڵ���)
	bool	mDepthWrite{ true };	// ���д��

	// polygonOffset��أ����ƫ�ƣ�
	bool			mPolygonOffset{ false };
	unsigned int	mPolygonOffsetType{ GL_POLYGON_OFFSET_FILL };
	float			mFactor{ 0.0f };	// ���б������
	float			mUnit{ 0.0f };		// ƫ������

	// stencil��أ�ģ����ԣ�
	bool			mStencilTest{ false };

	unsigned int	mSFail{ GL_KEEP };		// ģ�����ʧ������ô��
	unsigned int	mZFail{ GL_KEEP };		// ģ�����ͨ������Ȳ���ʧ������ô��
	unsigned int	mPass{ GL_KEEP };		// ģ����Ժ���Ȳ��Զ�ͨ������ô��

	unsigned int	mStencilMask{ 0xFF };	// ����ģ��д��

	unsigned int	mStencilFunc{ GL_ALWAYS };	
	unsigned int	mStencilRef{ 0 };			// �ο���ֵ
	unsigned int	mStencilFuncMask{ 0xFF };

	// ��ɫ���
	bool			mBlend{ false };				
	unsigned int	mSFactor{ GL_SRC_ALPHA };				// Դ��ɫʹ��ʲôֵ��Ϊ͸����
	unsigned int	mDFactor{ GL_ONE_MINUS_SRC_ALPHA };		// Ŀ����ɫʹ��ʲôֵ��Ϊ͸����
	float			mOpacity{ 1.0f };						// ����͸����

	// ���޳�
	bool		mFaceCulling{ false };
	unsigned int mFrontFace{ GL_CCW };		// ����Ķ��壨Ĭ����ʱ��Ϊ���棩
	unsigned int mCullFace{ GL_BACK };		// �޳�����

};	