#pragma once

#include "camera.hpp"

class CameraControl
{
public:
	CameraControl();
	~CameraControl();

	// 鼠标（左/中/右键）
	virtual void onMouse(int button, int action, float xpos, float ypos);	
	// 鼠标移动的情况下
	virtual void onCursor(double xpos, double ypos);
	// 键盘
	virtual void onKey(int key, int action, int mods);	
	virtual void update();
	// 中键滚轮放缩图片 ，offset 为1或-1
	virtual void onScroll(double offset);	

	void setCamera(Camera* camera);
	// 敏感度
	void setSensitivity(float s);
	void setScaleSpeed(float s);
protected:
	// 鼠标按键状态
	bool mLeftMouseDown{ false };
	bool mRightMouseDown{ false };
	bool mMiddleMouseDown{ false };
	// 当前鼠标的位置
	float mCurrentX{ 0.0f };
	float mCurrentY{ 0.0f };
	// 记录键盘相关按键的按下状态
	std::map<int, bool> mKeyMap;
	// 敏感度
	float mSensitivity{ 0.2f };
	// 存储当前控制的那架摄像机
	Camera* mCamera{ nullptr };
	// 记录相机缩放的速度
	float mScaleSpeed{ 0.2f };
};