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
	// 1. �жϵ�ǰ�İ����Ƿ���
	bool pressed = (action == GLFW_PRESS)? true : false;
	// 2. ������£���¼��ǰ���µ�λ��
	if (pressed)
	{
		mCurrentX = xpos;
		mCurrentY = ypos;
	}
	// 3. ���ݰ��µ���갴����ͬ�����İ���״̬
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
	// ���˵� GLFW_REPEAT��һֱ��ס��  ���
	if (action == GLFW_REPEAT)
	{
		return;
	}
	// 1. �жϵ�ǰ�İ������Ƿ���
	bool pressed = (action == GLFW_PRESS) ? true : false;
	// 2. ��¼��mKeyMap��
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