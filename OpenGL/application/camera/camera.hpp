#pragma once

#include "../../glframework/core.h"

class Camera
{
public:
    Camera();
    ~Camera();
    glm::mat4 getViewMatrix();
    virtual glm::mat4 getProjectionMatrix();
    virtual void scale(float deltaScale);   // 每一帧比上一帧的缩放程度
public:
    glm::vec3 mPosition{ 0.0f,0.0f,5.0f };
    glm::vec3 mUp{ 0.0f,1.0f,0.0f };
    glm::vec3 mRight{ 1.0f,0.0f,0.0f };
};