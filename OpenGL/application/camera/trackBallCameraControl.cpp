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
	{	// ��������ĸ������
		//1. ���㾭�߸�γ����ת�������Ƕ�
		float deltaX = (xpos - mCurrentX)*mSensitivity;
		float deltaY = (ypos - mCurrentY)*mSensitivity;

		// 2. �ֿ�pitch��yaw����
		pitch(-deltaY);
		yaw(-deltaX);
	}
	else if (mMiddleMouseDown)
	{
		// 1. xy���ƫ����������������ƫ������
		// �����ƶ�����mRight��		�����ƶ�����mUp��
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
	// ����Right������ת
	auto mat = glm::rotate(glm::mat4(1.0f), glm::radians(angle), mCamera->mRight);

	// Ӱ�쵱ǰ�����up������λ��Position
	/*mCamera->mUp = mat*glm::vec4(mCamera->mUp, 0.0f);
	mCamera->mPosition = mat*glm::vec4(mCamera->mPosition, 1.0f) ;*/
	mCamera->mUp =  glm::vec4(mCamera->mUp, 0.0f)*mat;
	mCamera->mPosition =  glm::vec4(mCamera->mPosition, 1.0f) * mat;
}

void TrackBallCameraControl::yaw(float angle)
{
	// ������������ϵ��y����ת
	auto mat = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

	// Ӱ�쵱ǰ�����Position��Up��Right����
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