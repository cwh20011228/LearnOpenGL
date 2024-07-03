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
	void pitch(float angle);	// ������ת
	void yaw(float angle);		// ������ת
private:
	float mPitch{ 0.0f };		// ��¼����Ƕ�
	float mSpeed{ 0.01f };		// WASD�ƶ��ٶ�
};