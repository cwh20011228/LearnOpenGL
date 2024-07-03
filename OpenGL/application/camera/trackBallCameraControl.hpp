#pragma once

#include "cameraControl.hpp"

class TrackBallCameraControl:public CameraControl
{
public:
	TrackBallCameraControl();
	~TrackBallCameraControl();

	void onCursor(double xpos, double ypos) override;
	void onScroll(double offset) override;
private:
	void pitch(float angle);
	void yaw(float angle);
private:
	float mMoveSpeed{ 0.05f };
};