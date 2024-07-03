#include "camera.hpp"

Camera::Camera()
{

}

Camera::~Camera()
{

}

glm::mat4 Camera::getViewMatrix()
{
	// lookAt
	// eye:相机位置，使用mPostion代替
	// center:看向世界坐标系的哪个点
	// top：穹顶，用mUP代替
	glm::vec3 front = glm::cross(mUp ,mRight);
	glm::vec3 center =   mPosition + front ;

	return glm::lookAt(mPosition, center, mUp);
}

glm::mat4 Camera::getProjectionMatrix()
{
	return glm::identity<glm::mat4>();
}

void Camera::scale(float deltaScale)
{

}