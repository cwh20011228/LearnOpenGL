#pragma once

#include "camera.hpp"

class CameraControl
{
public:
	CameraControl();
	~CameraControl();

	// ��꣨��/��/�Ҽ���
	virtual void onMouse(int button, int action, float xpos, float ypos);	
	// ����ƶ��������
	virtual void onCursor(double xpos, double ypos);
	// ����
	virtual void onKey(int key, int action, int mods);	
	virtual void update();
	// �м����ַ���ͼƬ ��offset Ϊ1��-1
	virtual void onScroll(double offset);	

	void setCamera(Camera* camera);
	// ���ж�
	void setSensitivity(float s);
	void setScaleSpeed(float s);
protected:
	// ��갴��״̬
	bool mLeftMouseDown{ false };
	bool mRightMouseDown{ false };
	bool mMiddleMouseDown{ false };
	// ��ǰ����λ��
	float mCurrentX{ 0.0f };
	float mCurrentY{ 0.0f };
	// ��¼������ذ����İ���״̬
	std::map<int, bool> mKeyMap;
	// ���ж�
	float mSensitivity{ 0.2f };
	// �洢��ǰ���Ƶ��Ǽ������
	Camera* mCamera{ nullptr };
	// ��¼������ŵ��ٶ�
	float mScaleSpeed{ 0.2f };
};