#pragma once

#include "cameraControl.hpp"

class TrackBallCameraControl:public CameraControl
{
public:
	TrackBallCameraControl();
	~TrackBallCameraControl();

	// ����ƶ��������
	void onCursor(double xpos, double ypos) override;	
	// �м����ַ���ͼƬ ��offset Ϊ1��-1
	void onScroll(double offset) override;		
private:
	void pitch(float angle);
	void yaw(float angle);
private:
	float mMoveSpeed{ 0.05f };
};