#include "gameCameraControl.hpp"

GameCameraControl::GameCameraControl()
{

}

GameCameraControl::~GameCameraControl()
{

}

void GameCameraControl::onCursor(double xpos, double ypos)
{
	float deltaX = (xpos - mCurrentX) * mSensitivity;
	float deltaY = (ypos - mCurrentY) * mSensitivity;

	if (mRightMouseDown)
	{
		pitch(-deltaY);
		yaw(-deltaX);
	}
	mCurrentX = xpos;
	mCurrentY = ypos;
}

void GameCameraControl::pitch(float angle)
{
	mPitch += angle;
	if (mPitch > 89.0f || mPitch < -89.0f)
	{
		mPitch -= angle;
		return;
	}
	// 在GameCameraControl的情况下，pitch不会影响mPosition
	auto mat = glm::rotate(glm::mat4(1.0f), glm::radians(angle), mCamera->mRight);
	mCamera->mUp = glm::vec4(mCamera->mUp, 0.0f)*mat;
}

void GameCameraControl::yaw(float angle)
{
	auto mat = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	mCamera->mUp = glm::vec4(mCamera->mUp, 0.0f) * mat;
	mCamera->mRight = glm::vec4(mCamera->mRight, 0.0f) * mat;
}

void GameCameraControl::update()
{
	// direction:最终移动方向
	glm::vec3 direction(0.0f);

	auto front = glm::cross(mCamera->mUp, mCamera->mRight);
	auto right = mCamera->mRight;
	auto up = mCamera->mUp;

	if (mKeyMap[GLFW_KEY_W])
	{
		//direction += front;
		direction -= up;
	}

	if (mKeyMap[GLFW_KEY_S])
	{
		//direction -= front;
		direction += up;
	}

	if (mKeyMap[GLFW_KEY_A])
	{
		direction += right;
	}

	if (mKeyMap[GLFW_KEY_D])
	{
		direction -= right;
	}

	// 朝着z轴正方向走 （往外走）
	if (mKeyMap[GLFW_KEY_Q])
	{
		direction += front;
	}

	if (mKeyMap[GLFW_KEY_E])
	{
		direction -= front;
	}

	// 此时,direction向量有可能不为1的长度，也有可能是0的长度
	if (glm::length(direction)!=0)
	{
		direction = glm::normalize(direction);		// 归1化
		mCamera->mPosition += direction * mSpeed;
	}
}

void GameCameraControl::setSpeed(float s)
{
	mSpeed = s;
}