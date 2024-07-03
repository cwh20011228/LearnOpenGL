#include "glframework/core.h"
#include "glframework/shader.h"
#include <iostream>
#include <string>
#include <assert.h>
#include "wrapper/checkError.h"
#include "application/application.h"
#include "glframework/texture.h"

// �������+������
#include "application/camera/camera.hpp"
#include "application/camera/orthograhicCamera.hpp"
#include "application/camera/perspectiveCamera.hpp"

#include "application/camera/cameraControl.hpp"
#include "application/camera/trackBallCameraControl.hpp"
#include "application/camera/gameCameraControl.hpp"

#include "./glframework/geometry.hpp"

Geometry* geometry{ nullptr };
Shader* shader = nullptr;
//Texture* texture = nullptr;
//glm::mat4 tranform(1.0f);		//�任����

glm::mat4 tranformCute(1.0f);	
//glm::mat4 tranformGirl(1.0f);	
Texture* textureCute{ nullptr };
//Texture* textureGirl{ nullptr };

PerspectiveCamera* camera = nullptr;
//OrthographicCamera* camera = nullptr;
TrackBallCameraControl* cameraControl = nullptr;
//GameCameraControl* cameraControl = nullptr;


void onResize(int width, int height);
void onKey(int key,int action,int mods);			// ���̵��
void onMouse(int button, int action, int mods);		// ��갴��/̧��
void onCursor(double xpos, double ypos);			// ����ƶ�
void onScroll(double offset);						// ������

void prepareShader();
void render();		
void prepareVAO();
void prepareTexture();
void prepareCamera();		
void prepareState();		// ����Opengl��״̬


int main(void)
{
    if (!app->Init(800,600))
    {
        return -1;
    }

	// ������Ϣ�ص�����
    app->setResizeCallback(onResize);
	app->setKeyBoardCallback(onKey);
	app->setMouseCallBack(onMouse);
	app->setCursorCallBack(onCursor);
	app->setSrollCallBack(onScroll);


    // ����OpenGL�ӿ��Լ�������ɫ
    GL_CALL(glViewport(0, 0, 800, 600));
    GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));

	prepareShader();
	prepareVAO();
	prepareTexture();
	prepareCamera();
	prepareState();

    while (app->update())
    {
		cameraControl->update();
		render();
    }

    app->destory();


    return 0;
}



void onResize(int width, int height)
{
    GL_CALL(glViewport(0, 0, width, height));
    std::cout << "OnResize" << std::endl;
}

void onKey(int key, int action, int mods)
{
	cameraControl->onKey(key, action, mods);
}

void onMouse(int button, int action, int mods)
{
	double x=0, y=0;
	app->getCursorPosition(&x, &y);
	cameraControl->onMouse(button, action, x, y);
}

void onCursor(double xpos, double ypos)
{
	cameraControl->onCursor(xpos, ypos);
}

void onScroll(double offset)
{
	cameraControl->onScroll(offset);
}

void prepareShader()
{
	shader = new Shader("assets/shaders/vertex.glsl","assets/shaders/fragment.glsl");
}                  

void render()
{
	// ִ��openGL�����������
	GL_CALL(glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ));		

	// 1. �󶨵�ǰ��program
	GL_CALL(shader->begin());
	
	// ����uniform������
	GL_CALL(shader->setInt("sampler", 0));	// ������ȥ0������Ԫ������ɫ����
	GL_CALL(shader->setMatrix4x4("transform", tranformCute));
	GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
	GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

	
	// �󶨵�ǰ��vao
	GL_CALL(glBindVertexArray(geometry->getVao()));

	// 3. ��������ָ��
	// ��һ�λ���
	textureCute->bind();	// ��̬�Ļ����������
	GL_CALL(glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0));

	//// �ڶ��λ���
	//textureGirl->bind();		// ��̬�Ļ����������
	//tranformGirl = glm::translate(glm::mat4(1.0f), glm::vec3(0.8f,0.0f,-1.0f));
	//GL_CALL(shader->setMatrix4x4("transform", tranformGirl));
	//GL_CALL(glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0));

	// 4. ���ǰ��vao ��program
	GL_CALL(glBindVertexArray(0));
	GL_CALL(shader->end());
}

void prepareVAO()
{
	//geometry=Geometry::createBox(6.0f);
	geometry = Geometry::createSphere(6.0f);
}

void prepareTexture()
{
	textureCute = new Texture("./assets/textures/mars.jpg", 0);
	//textureGirl = new Texture("./assets/textures/girl.png", 0);
}
	

void prepareCamera()
{
	camera = new PerspectiveCamera(	60.0f,
									(float)(app->getWidth()) / (float)(app->getHeight()), 
									0.1f, 
									1000.0f);
	/*float left = -6.0f;
	float right = 6.0f;
	float Top = 6.0f;
	float Bottom = -6.0f;
	float Far = 6.0f;
	float Near = -6.0f;
	camera = new OrthographicCamera(left,right,Top,Bottom,Near,Far);*/

	cameraControl = new TrackBallCameraControl();
	//cameraControl = new GameCameraControl();
	cameraControl->setCamera(camera);
	cameraControl->setSensitivity(0.2f);
}

void prepareState()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}
