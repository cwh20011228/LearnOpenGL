#include "cameraControl.hpp"
#include <iostream>


CameraControl::CameraControl()
{

}

CameraControl::~CameraControl()
{

}

void CameraControl::onMouse(int button, int action, float xpos,float ypos)
{
	// 1. 判断当前的按键是否按下
	bool pressed = (action == GLFW_PRESS)? true : false;
	// 2. 如果按下，记录当前按下的位置
	if (pressed)
	{
		mCurrentX = xpos;
		mCurrentY = ypos;
	}
	// 3. 根据按下的鼠标按键不同，更改按键状态
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT: 
		mLeftMouseDown = pressed;
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		mRightMouseDown = pressed;
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		mMiddleMouseDown = pressed;
		break;
	}
}

void CameraControl::onCursor(double xpos, double ypos)
{

}

void CameraControl::onKey(int key, int action, int mods)
{
	// 过滤掉 GLFW_REPEAT（一直按住）  情况
	if (action == GLFW_REPEAT)
	{
		return;
	}
	// 1. 判断当前的按键，是否按下
	bool pressed = (action == GLFW_PRESS) ? true : false;
	// 2. 记录在mKeyMap中
	mKeyMap[key] = pressed;
}

void CameraControl::update()
{

}

void CameraControl::setCamera(Camera* camera)
{
	mCamera = camera;
}

void CameraControl::setSensitivity(float s)
{
	mSensitivity = s;
}

void CameraControl::onScroll(double offset)
{

}

void CameraControl::setScaleSpeed(float s)
{
	mScaleSpeed = s;
}