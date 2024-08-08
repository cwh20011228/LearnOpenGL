#pragma once 

#include "camera.hpp"

class OrthographicCamera :public Camera
{
public:
	OrthographicCamera(	float Left,
						float Right,
						float Top,
						float Bottom,
						float Near,
						float Far);
	~OrthographicCamera();
	glm::mat4 getProjectionMatrix() override;
	void scale(float deltaScale) override;
private:
	float mLeft{0.0f};
	float mRight{ 0.0f };
	float mTop{ 0.0f };
	float mBottom{ 0.0f };
	float mScale{ 0.0f };	// 描述变大变小的程度

};
