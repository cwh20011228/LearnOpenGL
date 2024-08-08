#pragma once

#include "cameraControl.hpp"

class TrackBallCameraControl:public CameraControl
{
public:
	TrackBallCameraControl();
	~TrackBallCameraControl();

	// 鼠标移动的情况下
	void onCursor(double xpos, double ypos) override;	
	// 中键滚轮放缩图片 ，offset 为1或-1
	void onScroll(double offset) override;		
private:
	void pitch(float angle);
	void yaw(float angle);
private:
	float mMoveSpeed{ 0.05f };
};