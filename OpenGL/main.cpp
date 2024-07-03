#include "glframework/core.h"
#include "glframework/shader.h"
#include <iostream>
#include <string>
#include <assert.h>
#include "wrapper/checkError.h"
#include "application/application.h"
#include "glframework/texture.h"

// 引入相机+控制器
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
//glm::mat4 tranform(1.0f);		//变换矩阵

glm::mat4 tranformCute(1.0f);	
//glm::mat4 tranformGirl(1.0f);	
Texture* textureCute{ nullptr };
//Texture* textureGirl{ nullptr };

PerspectiveCamera* camera = nullptr;
//OrthographicCamera* camera = nullptr;
TrackBallCameraControl* cameraControl = nullptr;
//GameCameraControl* cameraControl = nullptr;


void onResize(int width, int height);
void onKey(int key,int action,int mods);			// 键盘点击
void onMouse(int button, int action, int mods);		// 鼠标按下/抬起
void onCursor(double xpos, double ypos);			// 鼠标移动
void onScroll(double offset);						// 鼠标滚轮

void prepareShader();
void render();		
void prepareVAO();
void prepareTexture();
void prepareCamera();		
void prepareState();		// 设置Opengl的状态


int main(void)
{
    if (!app->Init(800,600))
    {
        return -1;
    }

	// 设置消息回调函数
    app->setResizeCallback(onResize);
	app->setKeyBoardCallback(onKey);
	app->setMouseCallBack(onMouse);
	app->setCursorCallBack(onCursor);
	app->setSrollCallBack(onScroll);


    // 设置OpenGL视口以及清理颜色
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
	// 执行openGL画布清理操作
	GL_CALL(glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ));		

	// 1. 绑定当前的program
	GL_CALL(shader->begin());
	
	// 设置uniform变量的
	GL_CALL(shader->setInt("sampler", 0));	// 采样器去0号纹理单元采样颜色像素
	GL_CALL(shader->setMatrix4x4("transform", tranformCute));
	GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
	GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

	
	// 绑定当前的vao
	GL_CALL(glBindVertexArray(geometry->getVao()));

	// 3. 发出绘制指令
	// 第一次绘制
	textureCute->bind();	// 动态的换绑定纹理对象
	GL_CALL(glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0));

	//// 第二次绘制
	//textureGirl->bind();		// 动态的换绑定纹理对象
	//tranformGirl = glm::translate(glm::mat4(1.0f), glm::vec3(0.8f,0.0f,-1.0f));
	//GL_CALL(shader->setMatrix4x4("transform", tranformGirl));
	//GL_CALL(glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0));

	// 4. 解绑当前的vao 和program
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
