#include "glframework/core.h"
#include "glframework/shader.h"
#include <iostream>
#include <string>
#include <assert.h>
#include <cstdlib>
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

#include "./glframework/object.hpp"
#include "./glframework/mesh/mesh.hpp"
#include "./glframework/mesh/instanceMesh.hpp"
#include "./glframework/mesh/instanceUMesh.hpp"
#include "./glframework/geometry.hpp"
#include "glframework/scene.hpp"
// 材质
#include "./glframework/material/phongMaterial.hpp"
#include "glframework/material/whiteMaterial.hpp"
#include "glframework/material/depthMaterial.hpp"
#include "glframework/material/OpacityMaskMaterial.hpp"
#include "glframework/material/screenMaterial.hpp"
#include "glframework/material/cubeMaterial.hpp"
#include "glframework/material/PhongEnvMaterial.hpp"
#include "glframework/material/SpMapMaterial.hpp"
#include "glframework/material/PhongInstanceMaterial.hpp"
#include "glframework/material/PhongInstanceUMaterial.hpp"
#include "glframework/material/grassInstanceMaterial.hpp"
#include "glframework/renderer/renderer.hpp"

// Light
#include "glframework/light/pointLight.hpp"
#include "glframework/light/spotLight.hpp"

// GUI
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
// assimp 模型读取类
#include "application/assimpLoader.hpp"
#include "application/assimpInstanceLoader.hpp"
// 帧缓冲
#include "glframework/framebuffer/framebuffer.hpp"


// 渲染器 
Renderer* renderer{ nullptr };

// 场景
Scene* sceneOffscreen{ nullptr };
Scene* sceneInscreen{ nullptr };

// 帧缓冲
FrameBuffer* framebuffer{ nullptr };
// 材质
GrassInstanceMaterial* grassMaterial{ nullptr };

int WIDTH = 1920;
int HEIGHT = 1080;

// 灯光
DirectionalLight* dirLight{ nullptr };		// 平行光 
SpotLight* spotLight{ nullptr };			// 聚光灯
std::vector<PointLight*> pointLights{ };	// 点光源数组
AmbientLight* ambLight{ nullptr };			// 环境光

// 相机
Camera* camera = nullptr;
CameraControl* cameraControl = nullptr;

// GUI
glm::vec3 clearColor{0.0f,0.0f,0.0f};



void onResize(int width, int height);
// 键盘点击
void onKey(int key,int action,int mods);			
// 鼠标按下/抬起
void onMouse(int button, int action, int mods);		
// 鼠标移动
void onCursor(double xpos, double ypos);			
// 鼠标滚轮
void onScroll(double offset);						

void prepareCamera();
void prepare();	
void initIMGUI();
void renderIMGUI();

// 设置模型的颜色混合参数
void setModelBlend(Object* obj,bool blend,float opacity);
// 遍历模型层级，设置matrix数组
void setInstanceMatrix(Object* obj,int index,glm::mat4 matrix);
// 遍历模型层级，更新matrix数组到mMatrixVbo
void updateInstanceMatrix(Object* obj);
//	设置实例的材质
void setInstanceMaterial(Object* obj, Material* material);



int main(void)
{
    if (!glApp->Init(WIDTH,HEIGHT))
    {
        return -1;
    }

	// 设置消息回调函数
	glApp->setResizeCallback(onResize);
	glApp->setKeyBoardCallback(onKey);
	glApp->setMouseCallBack(onMouse);
	glApp->setCursorCallBack(onCursor);
	glApp->setSrollCallBack(onScroll);

    // 设置OpenGL视口以及清理颜色
    GL_CALL(glViewport(0, 0, WIDTH, HEIGHT));
    //GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
	//GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

	prepareCamera();
	prepare();
	initIMGUI();

    while (glApp->update())
    {
		cameraControl->update();
		renderer->setClearColor(clearColor);
		// pass01 将Box渲染到colorAttachment上，新的fbo上
		//renderer->render(sceneOffscreen, camera, dirLight, ambLight, framebuffer->mFBO);
		// pass02 将colorAttachment作为纹理，绘制到整个屏幕(默认的fbo是0)
		renderer->render(sceneInscreen,camera, dirLight, ambLight);
		renderIMGUI();
    }

	glApp->destory();

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
	glApp->getCursorPosition(&x, &y);
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

void prepare()
{
	renderer = new Renderer();
	sceneInscreen = new Scene();
	sceneOffscreen = new Scene();
	framebuffer = new FrameBuffer(WIDTH,HEIGHT);


#if 1
	//TODO: 实现草坪绘制

	//球形投影贴图（做天空盒）
	auto boxGeo = Geometry::createBox(1.0f);
	auto boxMat = new SpMapMaterial();
	boxMat->mDiffuse = new Texture("assets/textures/bk.jpg", 0);	// sphericalMap
	auto boxMesh = new Mesh(boxGeo, boxMat);
	sceneInscreen->addChild(boxMesh);

	// 草丛(x当作行，z当作列)
	int rNum = 40;
	int cNum = 40;

	auto grassModel = AssimpInstanceLoader::load("assets/obj/grassNew.obj", rNum * cNum);
	
	glm::mat4 translate;	// 平移矩阵
	glm::mat4 rotate;		// 旋转矩阵
	glm::mat4 transform;	// 总的矩阵

	srand(glfwGetTime());
	for (int r = 0; r < rNum; r++)
	{
		for (int c = 0; c < cNum; c++)
		{
			translate = glm::translate(glm::mat4(1.0f), glm::vec3(r * 0.2f, 0.0f, c * 0.2f));
			rotate = glm::rotate(glm::mat4(1.0f), glm::radians((float)(rand()%90)),glm::vec3(0.0f, 1.0f, 0.0f));
			transform = translate * rotate;		// 先做旋转，再做平移
			setInstanceMatrix(grassModel, r * cNum + c, transform);
		}
	}
	updateInstanceMatrix(grassModel);
	

	grassMaterial = new GrassInstanceMaterial();
	grassMaterial->mDiffuse = new Texture("assets/textures/GRASS.PNG", 0);
	grassMaterial->mOpacityMask = new Texture("assets/textures/grassMask.png", 1);
	grassMaterial->mCloudMask = new Texture("assets/textures/CLOUD.PNG", 2);
	grassMaterial->mBlend=true;
	grassMaterial->mDepthWrite = false;
	setInstanceMaterial(grassModel, grassMaterial);

	sceneInscreen->addChild(grassModel);

	auto house = AssimpLoader::load("assets/fbx/house.fbx");
	house->setScale(glm::vec3(0.5f));
	house->setPosition(glm::vec3(rNum * 0.2f / 2.0f, 0.4, cNum * 0.2f / 2.0f));	
	sceneInscreen->addChild(house);
	

	
#endif 


#if 0
	//TODO: 绘制实例(attribute方法)
	//球形投影贴图（做天空盒）
	auto boxGeo = Geometry::createBox(1.0f);
	auto boxMat = new SpMapMaterial();
	boxMat->mDiffuse = new Texture("assets/textures/bk.jpg", 0);	// sphericalMap
	auto boxMesh = new Mesh(boxGeo, boxMat);
	sceneInscreen->addChild(boxMesh);

	// 地球
	auto sphereGeo = Geometry::createSphere(4.0f);
	auto sphereMat = new PhongInstanceMaterial();
	sphereMat->mDiffuse = new Texture("assets/textures/earth.png", 0);
	auto sphereMesh = new InstanceMesh(sphereGeo, sphereMat, 3);
	glm::mat4 transform0 = glm::mat4(1.0f);
	glm::mat4 transform1 = glm::translate(glm::mat4(1.0f), glm::vec3(8.0f, 0.0f, 0.0f));
	glm::mat4 transform2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 8.0f, 0.0f));
	sphereMesh->mInstanceMatrices[0] = transform0;
	sphereMesh->mInstanceMatrices[1] = transform1;
	sphereMesh->mInstanceMatrices[2] = transform2;
	sphereMesh->updataMatrices();

	sceneInscreen->addChild(sphereMesh);

#endif


#if 0
	//TODO: 绘制实例(unifrom方法)
	//球形投影贴图（做天空盒）
	auto boxGeo = Geometry::createBox(1.0f);
	auto boxMat = new SpMapMaterial();
	boxMat->mDiffuse = new Texture("assets/textures/sphericalMap.png", 0);	// sphericalMap
	auto boxMesh = new Mesh(boxGeo, boxMat);
	sceneInscreen->addChild(boxMesh);

	// 地球
	auto sphereGeo = Geometry::createSphere(4.0f);
	auto sphereMat = new PhongInstanceUMaterial();
	sphereMat->mDiffuse = new Texture("assets/textures/earth.png", 0);
	auto sphereMesh = new InstanceUMesh(sphereGeo, sphereMat,3);
	glm::mat4 transform0 = glm::mat4(1.0f);
	glm::mat4 transform1 = glm::translate(glm::mat4(1.0f), glm::vec3(8.0f, 0.0f, 0.0f));
	glm::mat4 transform2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 8.0f, 0.0f));
	sphereMesh->mInstanceMatrices[0] = transform0;
	sphereMesh->mInstanceMatrices[1] = transform1;
	sphereMesh->mInstanceMatrices[2] = transform2;
	sceneInscreen->addChild(sphereMesh);

#endif


#if 0
	//TODO:球形投影贴图（做天空盒）
	auto boxGeo = Geometry::createBox(1.0f);
	auto boxMat = new SpMapMaterial();
	boxMat->mDiffuse = new Texture("assets/textures/sphericalMap.png", 0);	// sphericalMap
	auto boxMesh = new Mesh(boxGeo, boxMat);
	sceneInscreen->addChild(boxMesh);

	// 地球
	auto sphereGeo = Geometry::createSphere(4.0f);
	auto sphereMat = new PhongMaterial();
	sphereMat->mDiffuse = new Texture("assets/textures/earth.png", 0);
	auto sphereMesh = new Mesh(sphereGeo, sphereMat);
	sceneInscreen->addChild(sphereMesh);

#endif

#if 0
	//TODO: 天空盒作为环境光的来源
	// 天空盒
	std::vector<std::string> paths =
	{
		"assets/textures/skybox/right.jpg",
		"assets/textures/skybox/left.jpg",
		"assets/textures/skybox/top.jpg",
		"assets/textures/skybox/bottom.jpg",
		"assets/textures/skybox/back.jpg",
		"assets/textures/skybox/front.jpg"
	};

	auto boxGeo = Geometry::createBox(1.0f);
	auto boxMat = new CubeMaterial();
	boxMat->mDiffuse = new Texture(paths, 0);
	boxMat->mDepthWrite = false;
	auto boxMesh = new Mesh(boxGeo, boxMat);
	sceneInscreen->addChild(boxMesh);

	// 地球
	auto sphereGeo = Geometry::createSphere(4.0f);
	auto sphereMat = new PhongEnvMaterial();
	sphereMat->mDiffuse = new Texture("assets/textures/earth.png", 0);
	sphereMat->mEnv = new Texture(paths, 1);
	auto sphereMesh = new Mesh(sphereGeo, sphereMat);
	sceneInscreen->addChild(sphereMesh);
#endif

#if 0
	//TODO:如果先绘制了天空盒，需要关闭深度写入，防止阻挡后面的地球物体
	// 1. 在创建texture的时候，会发生y轴反转的情况，cubemap是不需要y轴反转的
	// 2. 天空盒必须第一个绘制（当作背景），其他物体随后绘制；否则天空盒会挡住其他物体
	// 通用方法：将剪裁空间坐标系的 Zc 恒等于 Wc，即天空盒的z深度恒为1
	// 注意：材质中的depthFunc应该为GL_LEQUAL
	
	// 地球
	auto sphereGeo = Geometry::createSphere(4.0f);
	auto sphereMat = new PhongMaterial();
	sphereMat->mDiffuse = new Texture("assets/textures/earth.png", 0);
	auto sphereMesh = new Mesh(sphereGeo, sphereMat);
	sceneInscreen->addChild(sphereMesh);

	// 天空盒
	std::vector<std::string> paths =
	{
		"assets/textures/skybox/right.jpg",
		"assets/textures/skybox/left.jpg",
		"assets/textures/skybox/top.jpg",
		"assets/textures/skybox/bottom.jpg",
		"assets/textures/skybox/back.jpg",
		"assets/textures/skybox/front.jpg"
	};

	auto boxGeo = Geometry::createBox(1.0f);
	auto boxMat = new CubeMaterial();
	boxMat->mDiffuse = new Texture(paths, 0);
	boxMat->mDepthWrite = false;
	auto boxMesh = new Mesh(boxGeo, boxMat);
	sceneInscreen->addChild(boxMesh);

#endif


#if 0
	//TODO:制作蒙鼓人 (mesh与camera位置相同)
	auto boxGeo = Geometry::createBox(1.0f);
	auto boxMat = new CubeMaterial();
	boxMat->mDiffuse = new Texture("assets/textures/cute.png", 0);
	auto mesh = new Mesh(boxGeo, boxMat);
	sceneInscreen->addChild(mesh);
#endif

#if 0
	//TODO:实验1：创建覆盖屏幕的矩形
	auto geometry = Geometry::createScreenPlane();
	auto material = new ScreenMaterial();
	material->mScreenTexture = new Texture("assets/textures/cute.png", 0);
	auto mesh = new Mesh(geometry, material);
	scene->addChild(mesh);
#endif

#if 0 模糊/锐化效果
	//TODO:实验2：
	// 场景1只有box，画到新建的fbo,离屏渲染
	auto boxGeo = Geometry::createBox(5.0f);
	auto boxMat = new PhongMaterial();
	boxMat->mDiffuse = new Texture("assets/textures/cute.png", 0);
	auto boxMesh = new Mesh(boxGeo, boxMat);
	sceneOffscreen->addChild(boxMesh);

	// 场景2只有贴屏图片，画到屏幕上
	auto geometry = Geometry::createScreenPlane();
	auto material = new ScreenMaterial();
	material->mScreenTexture = framebuffer->mColorAttachment;
	auto mesh = new Mesh(geometry, material);
	sceneInscreen->addChild(mesh);
#endif


#if 0
	//TODO: 面剔除实验
	auto plane = Geometry::createPlane(5.0f, 5.0f);
	auto material = new PhongMaterial();
	material->mDiffuse = new Texture("assets/textures/grass.jpg", 0);
	material->setFaceCull(true, GL_CCW, GL_BACK);
	auto mesh = new Mesh(plane, material);
	scene->addChild(mesh);
#endif

#if 0
	//TODO: 颜色混合实验1
	auto geometryBox = Geometry::createBox(5.0f);
	auto materialBox = new PhongMaterial();
	materialBox->mDiffuse = new Texture("assets/textures/cute.png", 0);
	auto meshBox = new Mesh(geometryBox, materialBox);
	scene->addChild(meshBox);

	auto geometryPlane = Geometry::createPlane(8.0f, 8.0f);
	auto materialPlane = new PhongMaterial();
	materialPlane->mDiffuse = new Texture("assets/textures/window.png", 0);
	auto meshPlane = new Mesh(geometryPlane, materialPlane); 
	meshPlane->setPosition(glm::vec3(0.0f, 0.0f, 4.0f));
	scene->addChild(meshPlane);
#endif

#if 0
	//TODO:颜色混合实验2：半透明物体要关闭深度写入
	// 不同物体之间（A和B，A在前面，B在后面(B是半透明物体)）,A会挡住B（深度检测）,
	// 在B内部，因为关闭深度写入了，深度缓冲中的值永远为1，那么内部的物体部分都可以绘制，前面挡不住后面
	auto geometryBox = Geometry::createBox(4.0f);
	auto materialBox = new PhongMaterial();
	materialBox->mDiffuse = new Texture("assets/textures/window.png", 0);
	materialBox->setBlend(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,0.3);
	//materialBox->mDepthWrite = false;		// 关闭深度写入
	auto meshBox = new Mesh(geometryBox, materialBox);
	meshBox->setPosition(glm::vec3(0.0f, 0.0f, 4.0f));
	scene->addChild(meshBox);

	auto model = AssimpLoader::load("assets/obj/bag/backpack.obj");
	scene->addChild(model);
#endif 

#if 0
	//TODO:颜色混合实验3:模型设置透明度
	auto model = AssimpLoader::load("assets/obj/bag/backpack.obj");
	setModelBlend(model, true, 0.5);
	scene->addChild(model);
#endif

#if 0
	//TODO:颜色混合实验4:渲染排序逻辑（先绘制不透明物体，透明物体从后往前绘制）
	// 背包
	auto model = AssimpLoader::load("assets/obj/bag/backpack.obj");
	setModelBlend(model, true, 0.2);
	scene->addChild(model);

	// 实体平面
	auto planeGeo = Geometry::createPlane(5.0, 5.0);
	auto planeMat = new PhongMaterial();
	planeMat->mDiffuse = new Texture("assets/textures/box.png", 0);
	auto planemesh = new Mesh(planeGeo, planeMat);
	planemesh->setPosition(glm::vec3(0.0f, 0.0f, 6.0f));
	scene->addChild(planemesh);

	// 半透明平面
	auto planeTrans = Geometry::createPlane(10.0, 10.0);
	auto planeMatTrans = new PhongMaterial();
	planeMatTrans->mDiffuse = new Texture("assets/textures/wall.jpg", 0);
	planeMatTrans->setBlend(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 0.4);
	auto planemeshTrans = new Mesh(planeTrans, planeMatTrans);
	planemeshTrans->setPosition(glm::vec3(0.0f, 0.0f, -6.0f));
	scene->addChild(planemeshTrans);

	// 实体平面
	auto planeGeo2 = Geometry::createPlane(5.0, 5.0);
	auto planeMat2 = new PhongMaterial();
	planeMat2->mDiffuse = new Texture("assets/textures/cute.png", 0);
	auto planemesh2 = new Mesh(planeGeo2, planeMat2);
	planemesh2->setPosition(glm::vec3(3.0f, 0.0f, 2.0f));
	scene->addChild(planemesh2);
#endif 

#if 0
	//TODO: 颜色混合实验4：透明度蒙版
	auto planeGeo = Geometry::createPlane(5.0f, 5.0f);
	auto planeMat = new OpacityMaskMaterial();
	planeMat->mDiffuse = new Texture("assets/textures/grass.jpg", 0);
	planeMat->mOpacityMask = new Texture("assets/textures/grassMask.png", 1);
	planeMat->mBlend = true;
	auto mesh = new Mesh(planeGeo, planeMat);
	scene->addChild(mesh);
#endif

#if 0
	//TODO: 实现草坪绘制
	auto grass = AssimpLoader::load("assets/fbx/grass.fbx");
	grass->setScale(glm::vec3(0.02));
	scene->addChild(grass);

	auto grassMat = new OpacityMaskMaterial();
	grassMat->mDiffuse = new Texture("assets/textures/grass.jpg", 0);
	grassMat->mOpacityMask = new Texture("assets/textures/grassMask.png", 1);
	grassMat->mBlend = true;
	grassMat->mDepthWrite = false;
	renderer->mGlobalMaterial = grassMat;
#endif

#if 0
	// TODO: 模板测试实验
	// TODO: 实验1：一个正方体+略大白色边界方块
	auto geometry = Geometry::createBox(5.0f);
	auto materialA = new PhongMaterial();
	materialA->mDiffuse = new Texture("./assets/textures/cute.png", 0);
	materialA->setStencilTest(true, GL_KEEP, GL_KEEP, GL_REPLACE, 0xFF, GL_ALWAYS, 1, 0xFF);
	auto meshA = new Mesh(geometry, materialA);
	scene->addChild(meshA);

	auto materialBound = new WhiteMaterial();
	materialBound->setStencilTest(true, GL_KEEP, GL_KEEP, GL_KEEP, 0x00, GL_NOTEQUAL, 1, 0xFF);
	auto meshABound = new Mesh(geometry, materialBound);
	meshABound->setPosition(meshA->getPosition());
	meshABound->setScale(glm::vec3(1.2f));
	scene->addChild(meshABound);
#endif 

#if 0
	//TODO: 实验2：绘制两个正常方块+二者边界方块
	// (如果两个物体需要共享边界，边界盒子的深度检测需要关闭)
	auto geometry = Geometry::createBox(5.0f);
	auto materialA = new PhongMaterial();
	materialA->mDiffuse = new Texture("./assets/textures/cute.png", 0);
	materialA->setStencilTest(true, GL_KEEP, GL_KEEP, GL_REPLACE, 0xFF, GL_ALWAYS, 1, 0xFF);

	auto meshA = new Mesh(geometry, materialA);
	scene->addChild(meshA);
	//// 开启模板测试
	//materialA->mStencilTest = true;
	//// 模板测试后的结果写入
	//materialA->mSFail = GL_KEEP;
	//materialA->mZFail = GL_KEEP;
	//materialA->mPass = GL_REPLACE;
	//// 控制写入
	//materialA->mStencilMask = 0xFF;
	//// 模板测试规则
	//materialA->mStencilFunc = GL_ALWAYS;
	//materialA->mStencilRef = 1;
	//materialA->mStencilFuncMask = 0xFF;

	// 边框A
	auto materialABound = new WhiteMaterial();
	materialABound->mDepthTest = false;
	materialABound->setStencilTest(true, GL_KEEP, GL_KEEP, GL_KEEP, 0x00, GL_NOTEQUAL, 1, 0xFF);

	auto meshABound = new Mesh(geometry, materialABound);
	meshABound->setPosition(meshA->getPosition());
	meshABound->setScale(glm::vec3(1.2f));
	scene->addChild(meshABound);
	
	// 物体C
	auto materialC = new PhongMaterial();
	materialC->mDiffuse = new Texture("./assets/textures/girl.png", 0);
	materialC->setStencilTest(true, GL_KEEP, GL_KEEP, GL_REPLACE, 0xFF, GL_ALWAYS, 1, 0xFF);
	auto meshC = new Mesh(geometry, materialC);
	meshC->setPosition(glm::vec3(4.0f, 2.0f, 2.0f));
	scene->addChild(meshC);

	// 边框C
	auto materialCBound = new WhiteMaterial();
	materialCBound->mDepthTest = false;
	materialCBound->setStencilTest(true, GL_KEEP, GL_KEEP, GL_KEEP, 0x00, GL_NOTEQUAL, 1, 0xFF);

	auto meshCBound = new Mesh(geometry, materialCBound);
	meshCBound->setPosition(meshC->getPosition());
	meshCBound->setScale(glm::vec3(1.2f));
	scene->addChild(meshCBound);
#endif

#if 0
	//TODO: 深度偏移实验，创建平面AB
	auto geometryA = Geometry::createPlane(600.0f, 900.0f);
	auto materialA = new PhongMaterial();
	materialA->mDiffuse = new Texture("./assets/textures/cute.png", 0);
	auto meshA = new Mesh(geometryA, materialA);
	meshA->rotateX(-88.0f);
	scene->addChild(meshA);

	auto materialB = new PhongMaterial();
	materialB->mDiffuse = new Texture("./assets/textures/box.png", 0);
	materialB->mPolygonOffset = true;	// 开启深度偏移
	materialB->setFactor(1.0f);
	materialB->setUnit(1.0f);
	auto meshB = new Mesh(geometryA, materialB);
	meshB->setPosition(glm::vec3(0.0f, 0.0f, -0.5f));
	meshB->rotateX(-88.0f);
	scene->addChild(meshB);
#endif

#if 0
	//TODO: 实验线性深度值（这里要改远平面的距离，改小一些，例如100）
	auto geometry = Geometry::createPlane(5.0f, 5.0f);
	auto material = new DepthMaterial();
	auto meshA = new Mesh(geometry, material);
	scene->addChild(meshA);   
#endif

#if 0
	//TODO: 创建三个平面ABC(B是透明的，挡不住C)
	auto geometryA = Geometry::createPlane(5.0f, 5.0f);
	auto materialA = new PhongMaterial();
	materialA->mDiffuse = new Texture("./assets/textures/cute.png", 0);
	auto meshA = new Mesh(geometryA, materialA);
	scene->addChild(meshA);

	auto materialB = new PhongMaterial();
	materialB->mDiffuse = new Texture("./assets/textures/box.png", 0);
	materialB->mDepthWrite = false;		// 关闭深度缓存写入
	auto meshB = new Mesh(geometryA, materialB);
	meshB->setPosition(glm::vec3(2.0f, 1.5f, -0.000000001f));
	scene->addChild(meshB);

	auto materialC = new PhongMaterial();
	materialC->mDiffuse = new Texture("./assets/textures/girl.png", 0);
	auto meshC = new Mesh(geometryA, materialC);
	meshC->setPosition(glm::vec3(3.0f, 3.5f, -3.0f));
	scene->addChild(meshC);
#endif

#if 0
	//TODO: 三维模型导入
	//	./assets/fbx/Fist Fight B.fbx		./assets/fbx/test/test.fbx  
	//  ./assets/obj/bag/backpack.obj  ./assets/obj/nanosuit/nanosuit.obj	
	
	Object* testModel = AssimpLoader::load("./assets/obj/bag/backpack.obj");
	//testModel->setScale(glm::vec3(0.05f));
	scene->addChild(testModel);
#endif

#if 0
	// 1. 创建geometry
	//auto geometry = Geometry::createSphere(3.0f);
	auto geometry = Geometry::createBox(1.0f);
	Geometry* spGeometry = Geometry::createSphere(1.0f);
	Geometry* sp1Geometry = Geometry::createSphere(1.0f);

	//2. 创建一个material并且配置参数
	auto material = new PhongMaterial();
	material->mShiness = 64.0f;
	material->mDiffuse = new Texture("./assets/textures/box.png", 0);
	material->mSpecularMask = new Texture("./assets/textures/sp_mask.png", 1);

	//3. 生成mesh
	auto mesh = new Mesh(geometry, material);
	auto spMesh = new Mesh(spGeometry, material);
	auto sp1Mesh = new Mesh(sp1Geometry, material);
	spMesh->setPosition(glm::vec3(2.0f, 0.0f, 0.0f));
	sp1Mesh->setPosition(glm::vec3(-2.0f, 0.0f, 0.0f));

	// 创建父子关系

	scene->addChild(mesh);
	mesh->addChild(spMesh);
	mesh->addChild(sp1Mesh);
	
#endif

	// 光源
	//TODO: 平行光
	dirLight = new DirectionalLight();
	dirLight->mSpecularIntensity = 0.1f;

	//TODO:点光源  创建pointLights数据(4个点光源)
	auto pointLight1 = new PointLight();
	pointLight1->setPosition(glm::vec3(0.0f, 0.0f, -1.0f));
	pointLight1->mColor = glm::vec3(1.0f, 0.0f, 0.0f);
	pointLight1->mK2 = 0.0f;
	pointLight1->mK1 = 0.0f;
	pointLight1->mKc = 1.0f;
	pointLights.push_back(pointLight1);

	auto pointLight2 = new PointLight();
	pointLight2->setPosition(glm::vec3(0.0f, 1.0f, 0.0f));
	pointLight2->mColor = glm::vec3(0.0f, 1.0f, 0.0f);
	pointLight2->mK2 = 0.0f;
	pointLight2->mK1 = 0.0f;
	pointLight2->mKc = 1.0f;
	pointLights.push_back(pointLight2);

	auto pointLight3 = new PointLight();
	pointLight3->setPosition(glm::vec3(0.0f, -1.0f, 0.0f));
	pointLight3->mColor = glm::vec3(0.0f, 0.0f, 1.0f);
	pointLight3->mK2 = 0.0f;
	pointLight3->mK1 = 0.0f;
	pointLight3->mKc = 1.0f;
	pointLights.push_back(pointLight3);

	auto pointLight4 = new PointLight();
	pointLight4->setPosition(glm::vec3(0.0f, 0.0f, -1.0f));
	pointLight4->mColor = glm::vec3(1.0f, 1.0f, 0.0f);
	pointLight4->mK2 = 0.0f;
	pointLight4->mK1 = 0.0f;
	pointLight4->mKc = 1.0f;
	pointLights.push_back(pointLight4);

	//TODO: 环境光
	ambLight = new AmbientLight();
	ambLight->mColor = glm::vec3(0.5f);
}

void prepareCamera()
{
	camera = new PerspectiveCamera(	60.0f,
									(float)(glApp->getWidth()) / (float)(glApp->getHeight()),
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
	cameraControl->setSensitivity(0.1f);
	cameraControl->setScaleSpeed(0.8f);
	
}

void initIMGUI()
{
	ImGui::CreateContext();		// 创建imgui上下文
	ImGui::StyleColorsDark();	// 选择一个主题

	// 设置ImGui和GLFW和Opengl的绑定
	ImGui_ImplGlfw_InitForOpenGL(glApp->getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 460");
}

void renderIMGUI()
{
	// 1. 开启当前的IMGUI渲染
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	

	// 2. 决定当前的GUI上面有哪些控件（从上到下）
	ImGui::Begin("GrassMaterialEditor");

	ImGui::Text("GrassColor");
	ImGui::SliderFloat("UVScale", &grassMaterial->mUVscale, 0.0f, 100.0f);
	ImGui::InputFloat("Brightness", &grassMaterial->mBrightness);

	ImGui::Text("Wind");
	ImGui::InputFloat("WindScale", &grassMaterial->mWindScale);
	ImGui::InputFloat("PhaseScale", &grassMaterial->mPhaseScale);
	ImGui::ColorEdit3("WindDriection",reinterpret_cast<float*>(& grassMaterial->mWindDirection));

	ImGui::Text("Cloud");
	ImGui::SliderFloat("CloudUVScale", &grassMaterial->mCloudUVScale, 0.0f, 100.0f);
	ImGui::SliderFloat("CloudSpeed", &grassMaterial->mCloudSpeed, 0.0f, 1.0f);
	ImGui::SliderFloat("CloudLerp", &grassMaterial->mCloudLerp, 0.0f, 1.0f);
	ImGui::ColorEdit3("CloudWhiteColor", reinterpret_cast<float*>(&grassMaterial->mCloudWhiteColor));
	ImGui::ColorEdit3("CloudBlackColor", reinterpret_cast<float*>(&grassMaterial->mCloudBlackColor));

	ImGui::Text("Light");
	ImGui::InputFloat("Intensity", &dirLight->mInstensity);

	ImGui::End();

	// 3.执行UI渲染
	ImGui::Render();
	// 获取窗体的宽，高
	int display_w, display_h;
	glfwGetFramebufferSize(glApp->getWindow(), &display_w, &display_h);
	// 重置视口大小
	glViewport(0, 0, display_w, display_h);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

// 设置模型的颜色混合参数
void setModelBlend(Object* obj, bool blend, float opacity)
{
	if (obj->getType()==ObjectType::Mesh)
	{
		Mesh* mesh = (Mesh*)obj;
		Material* mat = mesh->mMaterial;
		mat->setBlend(blend, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, opacity);
		mat->mDepthWrite = false;
	}
	auto children = obj->getChildren();
	for (int i = 0; i < children.size(); i++)
	{
		setModelBlend(children[i], blend, opacity);
	}
}

// 遍历模型层级，设置matrix数组（用递归）
void setInstanceMatrix(Object* obj, int index, glm::mat4 matrix)
{
	if (obj->getType() == ObjectType::InstanceMesh)
	{
		InstanceMesh* instanceMesh = reinterpret_cast<InstanceMesh*>(obj);
		instanceMesh->mInstanceMatrices[index] = matrix;
	}
	auto children = obj->getChildren();
	for (auto val:children)
	{
		setInstanceMatrix(val, index, matrix);
	}
}

// 遍历模型层级，更新matrix数组到mMatrixVbo
void updateInstanceMatrix(Object* obj)
{
	if (obj->getType() == ObjectType::InstanceMesh)
	{
		InstanceMesh* instanceMesh = reinterpret_cast<InstanceMesh*>(obj);
		instanceMesh->updataMatrices();
	}
	auto children = obj->getChildren();
	for (auto val : children)
	{
		updateInstanceMatrix(val);
	}
}

//	设置实例的材质
void setInstanceMaterial(Object* obj, Material* material)
{
	if (obj->getType() == ObjectType::InstanceMesh)
	{
		InstanceMesh* instanceMesh = reinterpret_cast<InstanceMesh*>(obj);
		instanceMesh->mMaterial = material;
	}
	auto children = obj->getChildren();
	for (auto val : children)
	{
		setInstanceMaterial(val,material);
	}
}