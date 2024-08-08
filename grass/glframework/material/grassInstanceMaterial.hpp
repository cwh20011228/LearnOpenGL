#pragma once

#include "../core.h"
#include "../texture.h"
#include "./material.hpp"

// 使用attribute方式
class GrassInstanceMaterial :public Material
{
public:
	GrassInstanceMaterial();
	~GrassInstanceMaterial();

public:
	void setUVScale(float uvscale) { mUVscale = uvscale; }
public:
	Texture* mDiffuse{ nullptr };		// 漫反射颜色
	Texture* mOpacityMask{ nullptr };	// 透明蒙版
	

	float mShiness{ 1.0f };				// 光斑大小

	// 草地贴图特性
	float mUVscale{ 1.0 };
	float mBrightness{ 1.0f };	//草坪亮度
	// 风力方向
	glm::vec3 mWindDirection{ 1.0f,1.0f,1.0f };
	// 风力强度
	float mWindScale{ 0.2f };	
	// 相位大小
	float mPhaseScale{ 1.0f };

	// 云朵相关
	Texture*	mCloudMask			{ nullptr };		
	// 云层白色和黑色区域颜色
	glm::vec3	mCloudWhiteColor	{ 1.0f,0.0f,0.0f };
	glm::vec3	mCloudBlackColor	{ 0.0f,0.0f,1.0f };
	float		mCloudUVScale		{ 1.0f };
	float		mCloudSpeed			{ 0.1f };					// 云朵运动速度
	float		mCloudLerp			{ 0.5f };					// 云层占的比例

};