#include "trackBallCameraControl.hpp"

TrackBallCameraControl::TrackBallCameraControl()
{

}

TrackBallCameraControl::~TrackBallCameraControl()
{

}

void TrackBallCameraControl::onCursor(double xpos, double ypos)
{
	if (mLeftMouseDown)
	{	// 调整相机的各类参数
		//1. 计算经线跟纬线旋转的增量角度
		float deltaX = (xpos - mCurrentX)*mSensitivity;
		float deltaY = (ypos - mCurrentY)*mSensitivity;

		// 2. 分开pitch跟yaw计算
		pitch(-deltaY);
		yaw(-deltaX);
	}
	else if (mMiddleMouseDown)
	{
		// 1. xy轴的偏移量（横向和纵向的偏移量）
		// 横向移动沿着mRight轴		纵向移动沿着mUp轴
		float deltaX = (xpos - mCurrentX) * mMoveSpeed;
		float deltaY = (ypos - mCurrentY) * mMoveSpeed;

		mCamera->mPosition += mCamera->mUp * deltaY;
		mCamera->mPosition -= mCamera->mRight * deltaX;
	}
	mCurrentX = xpos;
	mCurrentY = ypos;
}

void TrackBallCameraControl::pitch(float angle)
{
	// 绕着Right向量旋转
	auto mat = glm::rotate(glm::mat4(1.0f), glm::radians(angle), mCamera->mRight);

	// 影响当前相机的up向量和位置Position
	/*mCamera->mUp = mat*glm::vec4(mCamera->mUp, 0.0f);
	mCamera->mPosition = mat*glm::vec4(mCamera->mPosition, 1.0f) ;*/
	mCamera->mUp =  glm::vec4(mCamera->mUp, 0.0f)*mat;
	mCamera->mPosition =  glm::vec4(mCamera->mPosition, 1.0f) * mat;
}

void TrackBallCameraControl::yaw(float angle)
{
	// 绕着世界坐标系的y轴旋转
	auto mat = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

	// 影响当前相机的Position，Up，Right向量
	/*mCamera->mPosition = mat* glm::vec4(mCamera->mPosition, 1.0f);
	mCamera->mUp = mat*glm::vec4(mCamera->mUp, 0.0f) ;
	mCamera->mRight =mat* glm::vec4(mCamera->mRight, 0.0f);*/
	mCamera->mPosition =  glm::vec4(mCamera->mPosition, 1.0f)*mat;
	mCamera->mUp = glm::vec4(mCamera->mUp, 0.0f) * mat;
	mCamera->mRight =  glm::vec4(mCamera->mRight, 0.0f) * mat;
}

void TrackBallCameraControl::onScroll(double offset)
{
	mCamera->scale(mScaleSpeed * offset);
}