#pragma once

#include "../core.h"
#include "../texture.h"
#include "./material.hpp"

// ʹ��attribute��ʽ
class GrassInstanceMaterial :public Material
{
public:
	GrassInstanceMaterial();
	~GrassInstanceMaterial();

public:
	void setUVScale(float uvscale) { mUVscale = uvscale; }
public:
	Texture* mDiffuse{ nullptr };		// ��������ɫ
	Texture* mOpacityMask{ nullptr };	// ͸���ɰ�
	

	float mShiness{ 1.0f };				// ��ߴ�С

	// �ݵ���ͼ����
	float mUVscale{ 1.0 };
	float mBrightness{ 1.0f };	//��ƺ����
	// ��������
	glm::vec3 mWindDirection{ 1.0f,1.0f,1.0f };
	// ����ǿ��
	float mWindScale{ 0.2f };	
	// ��λ��С
	float mPhaseScale{ 1.0f };

	// �ƶ����
	Texture*	mCloudMask			{ nullptr };		
	// �Ʋ��ɫ�ͺ�ɫ������ɫ
	glm::vec3	mCloudWhiteColor	{ 1.0f,0.0f,0.0f };
	glm::vec3	mCloudBlackColor	{ 0.0f,0.0f,1.0f };
	float		mCloudUVScale		{ 1.0f };
	float		mCloudSpeed			{ 0.1f };					// �ƶ��˶��ٶ�
	float		mCloudLerp			{ 0.5f };					// �Ʋ�ռ�ı���

};