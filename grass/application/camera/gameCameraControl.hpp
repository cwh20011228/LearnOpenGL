#pragma once

#include "cameraControl.hpp"

class GameCameraControl:public CameraControl
{
public:
	GameCameraControl();
	~GameCameraControl();

	void setSpeed(float s);

	void onCursor(double xpos,double ypos) override;
	void update() override;
private:
	void pitch(float angle);	// 上下旋转
	void yaw(float angle);		// 左右旋转
private:
	float mPitch{ 0.0f };		// 记录总体角度
	float mSpeed{ 0.01f };		// WASD移动速度
};