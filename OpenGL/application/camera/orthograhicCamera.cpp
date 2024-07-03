#include "orthograhicCamera.hpp"

OrthographicCamera::OrthographicCamera(	float Left,
										float Right,
										float Top,
										float Bottom,
										float Near,
										float Far)
{
	mLeft=Left;
	mRight=Right;
	mTop=Top;
	mBottom=Bottom;
	mNear=Near;
	mFar=Far;
}

OrthographicCamera::~OrthographicCamera()
{

}

glm::mat4 OrthographicCamera::getProjectionMatrix()
{
	float scale = std::pow(2.0, mScale);
	return glm::ortho(mLeft * scale, mRight * scale, mTop * scale, mBottom * scale, mNear, mFar);
}

void OrthographicCamera::scale(float deltaScale)
{
	mScale += deltaScale;
}