#include "glframework/core.h"
#include "glframework/shader.h"
#include <iostream>
#include <string>
#include <assert.h>
#include <cstdlib>
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

#include "./glframework/object.hpp"
#include "./glframework/mesh/mesh.hpp"
#include "./glframework/mesh/instanceMesh.hpp"
#include "./glframework/mesh/instanceUMesh.hpp"
#include "./glframework/geometry.hpp"
#include "glframework/scene.hpp"
// ����
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
// assimp ģ�Ͷ�ȡ��
#include "application/assimpLoader.hpp"
#include "application/assimpInstanceLoader.hpp"
// ֡����
#include "glframework/framebuffer/framebuffer.hpp"


// ��Ⱦ�� 
Renderer* renderer{ nullptr };

// ����
Scene* sceneOffscreen{ nullptr };
Scene* sceneInscreen{ nullptr };

// ֡����
FrameBuffer* framebuffer{ nullptr };
// ����
GrassInstanceMaterial* grassMaterial{ nullptr };

int WIDTH = 1920;
int HEIGHT = 1080;

// �ƹ�
DirectionalLight* dirLight{ nullptr };		// ƽ�й� 
SpotLight* spotLight{ nullptr };			// �۹��
std::vector<PointLight*> pointLights{ };	// ���Դ����
AmbientLight* ambLight{ nullptr };			// ������

// ���
Camera* camera = nullptr;
CameraControl* cameraControl = nullptr;

// GUI
glm::vec3 clearColor{0.0f,0.0f,0.0f};



void onResize(int width, int height);
// ���̵��
void onKey(int key,int action,int mods);			
// ��갴��/̧��
void onMouse(int button, int action, int mods);		
// ����ƶ�
void onCursor(double xpos, double ypos);			
// ������
void onScroll(double offset);						

void prepareCamera();
void prepare();	
void initIMGUI();
void renderIMGUI();

// ����ģ�͵���ɫ��ϲ���
void setModelBlend(Object* obj,bool blend,float opacity);
// ����ģ�Ͳ㼶������matrix����
void setInstanceMatrix(Object* obj,int index,glm::mat4 matrix);
// ����ģ�Ͳ㼶������matrix���鵽mMatrixVbo
void updateInstanceMatrix(Object* obj);
//	����ʵ���Ĳ���
void setInstanceMaterial(Object* obj, Material* material);



int main(void)
{
    if (!glApp->Init(WIDTH,HEIGHT))
    {
        return -1;
    }

	// ������Ϣ�ص�����
	glApp->setResizeCallback(onResize);
	glApp->setKeyBoardCallback(onKey);
	glApp->setMouseCallBack(onMouse);
	glApp->setCursorCallBack(onCursor);
	glApp->setSrollCallBack(onScroll);

    // ����OpenGL�ӿ��Լ�������ɫ
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
		// pass01 ��Box��Ⱦ��colorAttachment�ϣ��µ�fbo��
		//renderer->render(sceneOffscreen, camera, dirLight, ambLight, framebuffer->mFBO);
		// pass02 ��colorAttachment��Ϊ�������Ƶ�������Ļ(Ĭ�ϵ�fbo��0)
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
	//TODO: ʵ�ֲ�ƺ����

	//����ͶӰ��ͼ������պУ�
	auto boxGeo = Geometry::createBox(1.0f);
	auto boxMat = new SpMapMaterial();
	boxMat->mDiffuse = new Texture("assets/textures/bk.jpg", 0);	// sphericalMap
	auto boxMesh = new Mesh(boxGeo, boxMat);
	sceneInscreen->addChild(boxMesh);

	// �ݴ�(x�����У�z������)
	int rNum = 40;
	int cNum = 40;

	auto grassModel = AssimpInstanceLoader::load("assets/obj/grassNew.obj", rNum * cNum);
	
	glm::mat4 translate;	// ƽ�ƾ���
	glm::mat4 rotate;		// ��ת����
	glm::mat4 transform;	// �ܵľ���

	srand(glfwGetTime());
	for (int r = 0; r < rNum; r++)
	{
		for (int c = 0; c < cNum; c++)
		{
			translate = glm::translate(glm::mat4(1.0f), glm::vec3(r * 0.2f, 0.0f, c * 0.2f));
			rotate = glm::rotate(glm::mat4(1.0f), glm::radians((float)(rand()%90)),glm::vec3(0.0f, 1.0f, 0.0f));
			transform = translate * rotate;		// ������ת������ƽ��
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
	//TODO: ����ʵ��(attribute����)
	//����ͶӰ��ͼ������պУ�
	auto boxGeo = Geometry::createBox(1.0f);
	auto boxMat = new SpMapMaterial();
	boxMat->mDiffuse = new Texture("assets/textures/bk.jpg", 0);	// sphericalMap
	auto boxMesh = new Mesh(boxGeo, boxMat);
	sceneInscreen->addChild(boxMesh);

	// ����
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
	//TODO: ����ʵ��(unifrom����)
	//����ͶӰ��ͼ������պУ�
	auto boxGeo = Geometry::createBox(1.0f);
	auto boxMat = new SpMapMaterial();
	boxMat->mDiffuse = new Texture("assets/textures/sphericalMap.png", 0);	// sphericalMap
	auto boxMesh = new Mesh(boxGeo, boxMat);
	sceneInscreen->addChild(boxMesh);

	// ����
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
	//TODO:����ͶӰ��ͼ������պУ�
	auto boxGeo = Geometry::createBox(1.0f);
	auto boxMat = new SpMapMaterial();
	boxMat->mDiffuse = new Texture("assets/textures/sphericalMap.png", 0);	// sphericalMap
	auto boxMesh = new Mesh(boxGeo, boxMat);
	sceneInscreen->addChild(boxMesh);

	// ����
	auto sphereGeo = Geometry::createSphere(4.0f);
	auto sphereMat = new PhongMaterial();
	sphereMat->mDiffuse = new Texture("assets/textures/earth.png", 0);
	auto sphereMesh = new Mesh(sphereGeo, sphereMat);
	sceneInscreen->addChild(sphereMesh);

#endif

#if 0
	//TODO: ��պ���Ϊ���������Դ
	// ��պ�
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

	// ����
	auto sphereGeo = Geometry::createSphere(4.0f);
	auto sphereMat = new PhongEnvMaterial();
	sphereMat->mDiffuse = new Texture("assets/textures/earth.png", 0);
	sphereMat->mEnv = new Texture(paths, 1);
	auto sphereMesh = new Mesh(sphereGeo, sphereMat);
	sceneInscreen->addChild(sphereMesh);
#endif

#if 0
	//TODO:����Ȼ�������պУ���Ҫ�ر����д�룬��ֹ�赲����ĵ�������
	// 1. �ڴ���texture��ʱ�򣬻ᷢ��y�ᷴת�������cubemap�ǲ���Ҫy�ᷴת��
	// 2. ��պб����һ�����ƣ��������������������������ƣ�������պлᵲס��������
	// ͨ�÷����������ÿռ�����ϵ�� Zc ����� Wc������պе�z��Ⱥ�Ϊ1
	// ע�⣺�����е�depthFuncӦ��ΪGL_LEQUAL
	
	// ����
	auto sphereGeo = Geometry::createSphere(4.0f);
	auto sphereMat = new PhongMaterial();
	sphereMat->mDiffuse = new Texture("assets/textures/earth.png", 0);
	auto sphereMesh = new Mesh(sphereGeo, sphereMat);
	sceneInscreen->addChild(sphereMesh);

	// ��պ�
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
	//TODO:�����ɹ��� (mesh��cameraλ����ͬ)
	auto boxGeo = Geometry::createBox(1.0f);
	auto boxMat = new CubeMaterial();
	boxMat->mDiffuse = new Texture("assets/textures/cute.png", 0);
	auto mesh = new Mesh(boxGeo, boxMat);
	sceneInscreen->addChild(mesh);
#endif

#if 0
	//TODO:ʵ��1������������Ļ�ľ���
	auto geometry = Geometry::createScreenPlane();
	auto material = new ScreenMaterial();
	material->mScreenTexture = new Texture("assets/textures/cute.png", 0);
	auto mesh = new Mesh(geometry, material);
	scene->addChild(mesh);
#endif

#if 0 ģ��/��Ч��
	//TODO:ʵ��2��
	// ����1ֻ��box�������½���fbo,������Ⱦ
	auto boxGeo = Geometry::createBox(5.0f);
	auto boxMat = new PhongMaterial();
	boxMat->mDiffuse = new Texture("assets/textures/cute.png", 0);
	auto boxMesh = new Mesh(boxGeo, boxMat);
	sceneOffscreen->addChild(boxMesh);

	// ����2ֻ������ͼƬ��������Ļ��
	auto geometry = Geometry::createScreenPlane();
	auto material = new ScreenMaterial();
	material->mScreenTexture = framebuffer->mColorAttachment;
	auto mesh = new Mesh(geometry, material);
	sceneInscreen->addChild(mesh);
#endif


#if 0
	//TODO: ���޳�ʵ��
	auto plane = Geometry::createPlane(5.0f, 5.0f);
	auto material = new PhongMaterial();
	material->mDiffuse = new Texture("assets/textures/grass.jpg", 0);
	material->setFaceCull(true, GL_CCW, GL_BACK);
	auto mesh = new Mesh(plane, material);
	scene->addChild(mesh);
#endif

#if 0
	//TODO: ��ɫ���ʵ��1
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
	//TODO:��ɫ���ʵ��2����͸������Ҫ�ر����д��
	// ��ͬ����֮�䣨A��B��A��ǰ�棬B�ں���(B�ǰ�͸������)��,A�ᵲסB����ȼ�⣩,
	// ��B�ڲ�����Ϊ�ر����д���ˣ���Ȼ����е�ֵ��ԶΪ1����ô�ڲ������岿�ֶ����Ի��ƣ�ǰ�浲��ס����
	auto geometryBox = Geometry::createBox(4.0f);
	auto materialBox = new PhongMaterial();
	materialBox->mDiffuse = new Texture("assets/textures/window.png", 0);
	materialBox->setBlend(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,0.3);
	//materialBox->mDepthWrite = false;		// �ر����д��
	auto meshBox = new Mesh(geometryBox, materialBox);
	meshBox->setPosition(glm::vec3(0.0f, 0.0f, 4.0f));
	scene->addChild(meshBox);

	auto model = AssimpLoader::load("assets/obj/bag/backpack.obj");
	scene->addChild(model);
#endif 

#if 0
	//TODO:��ɫ���ʵ��3:ģ������͸����
	auto model = AssimpLoader::load("assets/obj/bag/backpack.obj");
	setModelBlend(model, true, 0.5);
	scene->addChild(model);
#endif

#if 0
	//TODO:��ɫ���ʵ��4:��Ⱦ�����߼����Ȼ��Ʋ�͸�����壬͸������Ӻ���ǰ���ƣ�
	// ����
	auto model = AssimpLoader::load("assets/obj/bag/backpack.obj");
	setModelBlend(model, true, 0.2);
	scene->addChild(model);

	// ʵ��ƽ��
	auto planeGeo = Geometry::createPlane(5.0, 5.0);
	auto planeMat = new PhongMaterial();
	planeMat->mDiffuse = new Texture("assets/textures/box.png", 0);
	auto planemesh = new Mesh(planeGeo, planeMat);
	planemesh->setPosition(glm::vec3(0.0f, 0.0f, 6.0f));
	scene->addChild(planemesh);

	// ��͸��ƽ��
	auto planeTrans = Geometry::createPlane(10.0, 10.0);
	auto planeMatTrans = new PhongMaterial();
	planeMatTrans->mDiffuse = new Texture("assets/textures/wall.jpg", 0);
	planeMatTrans->setBlend(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 0.4);
	auto planemeshTrans = new Mesh(planeTrans, planeMatTrans);
	planemeshTrans->setPosition(glm::vec3(0.0f, 0.0f, -6.0f));
	scene->addChild(planemeshTrans);

	// ʵ��ƽ��
	auto planeGeo2 = Geometry::createPlane(5.0, 5.0);
	auto planeMat2 = new PhongMaterial();
	planeMat2->mDiffuse = new Texture("assets/textures/cute.png", 0);
	auto planemesh2 = new Mesh(planeGeo2, planeMat2);
	planemesh2->setPosition(glm::vec3(3.0f, 0.0f, 2.0f));
	scene->addChild(planemesh2);
#endif 

#if 0
	//TODO: ��ɫ���ʵ��4��͸�����ɰ�
	auto planeGeo = Geometry::createPlane(5.0f, 5.0f);
	auto planeMat = new OpacityMaskMaterial();
	planeMat->mDiffuse = new Texture("assets/textures/grass.jpg", 0);
	planeMat->mOpacityMask = new Texture("assets/textures/grassMask.png", 1);
	planeMat->mBlend = true;
	auto mesh = new Mesh(planeGeo, planeMat);
	scene->addChild(mesh);
#endif

#if 0
	//TODO: ʵ�ֲ�ƺ����
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
	// TODO: ģ�����ʵ��
	// TODO: ʵ��1��һ��������+�Դ��ɫ�߽緽��
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
	//TODO: ʵ��2������������������+���߽߱緽��
	// (�������������Ҫ����߽磬�߽���ӵ���ȼ����Ҫ�ر�)
	auto geometry = Geometry::createBox(5.0f);
	auto materialA = new PhongMaterial();
	materialA->mDiffuse = new Texture("./assets/textures/cute.png", 0);
	materialA->setStencilTest(true, GL_KEEP, GL_KEEP, GL_REPLACE, 0xFF, GL_ALWAYS, 1, 0xFF);

	auto meshA = new Mesh(geometry, materialA);
	scene->addChild(meshA);
	//// ����ģ�����
	//materialA->mStencilTest = true;
	//// ģ����Ժ�Ľ��д��
	//materialA->mSFail = GL_KEEP;
	//materialA->mZFail = GL_KEEP;
	//materialA->mPass = GL_REPLACE;
	//// ����д��
	//materialA->mStencilMask = 0xFF;
	//// ģ����Թ���
	//materialA->mStencilFunc = GL_ALWAYS;
	//materialA->mStencilRef = 1;
	//materialA->mStencilFuncMask = 0xFF;

	// �߿�A
	auto materialABound = new WhiteMaterial();
	materialABound->mDepthTest = false;
	materialABound->setStencilTest(true, GL_KEEP, GL_KEEP, GL_KEEP, 0x00, GL_NOTEQUAL, 1, 0xFF);

	auto meshABound = new Mesh(geometry, materialABound);
	meshABound->setPosition(meshA->getPosition());
	meshABound->setScale(glm::vec3(1.2f));
	scene->addChild(meshABound);
	
	// ����C
	auto materialC = new PhongMaterial();
	materialC->mDiffuse = new Texture("./assets/textures/girl.png", 0);
	materialC->setStencilTest(true, GL_KEEP, GL_KEEP, GL_REPLACE, 0xFF, GL_ALWAYS, 1, 0xFF);
	auto meshC = new Mesh(geometry, materialC);
	meshC->setPosition(glm::vec3(4.0f, 2.0f, 2.0f));
	scene->addChild(meshC);

	// �߿�C
	auto materialCBound = new WhiteMaterial();
	materialCBound->mDepthTest = false;
	materialCBound->setStencilTest(true, GL_KEEP, GL_KEEP, GL_KEEP, 0x00, GL_NOTEQUAL, 1, 0xFF);

	auto meshCBound = new Mesh(geometry, materialCBound);
	meshCBound->setPosition(meshC->getPosition());
	meshCBound->setScale(glm::vec3(1.2f));
	scene->addChild(meshCBound);
#endif

#if 0
	//TODO: ���ƫ��ʵ�飬����ƽ��AB
	auto geometryA = Geometry::createPlane(600.0f, 900.0f);
	auto materialA = new PhongMaterial();
	materialA->mDiffuse = new Texture("./assets/textures/cute.png", 0);
	auto meshA = new Mesh(geometryA, materialA);
	meshA->rotateX(-88.0f);
	scene->addChild(meshA);

	auto materialB = new PhongMaterial();
	materialB->mDiffuse = new Texture("./assets/textures/box.png", 0);
	materialB->mPolygonOffset = true;	// �������ƫ��
	materialB->setFactor(1.0f);
	materialB->setUnit(1.0f);
	auto meshB = new Mesh(geometryA, materialB);
	meshB->setPosition(glm::vec3(0.0f, 0.0f, -0.5f));
	meshB->rotateX(-88.0f);
	scene->addChild(meshB);
#endif

#if 0
	//TODO: ʵ���������ֵ������Ҫ��Զƽ��ľ��룬��СһЩ������100��
	auto geometry = Geometry::createPlane(5.0f, 5.0f);
	auto material = new DepthMaterial();
	auto meshA = new Mesh(geometry, material);
	scene->addChild(meshA);   
#endif

#if 0
	//TODO: ��������ƽ��ABC(B��͸���ģ�����סC)
	auto geometryA = Geometry::createPlane(5.0f, 5.0f);
	auto materialA = new PhongMaterial();
	materialA->mDiffuse = new Texture("./assets/textures/cute.png", 0);
	auto meshA = new Mesh(geometryA, materialA);
	scene->addChild(meshA);

	auto materialB = new PhongMaterial();
	materialB->mDiffuse = new Texture("./assets/textures/box.png", 0);
	materialB->mDepthWrite = false;		// �ر���Ȼ���д��
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
	//TODO: ��άģ�͵���
	//	./assets/fbx/Fist Fight B.fbx		./assets/fbx/test/test.fbx  
	//  ./assets/obj/bag/backpack.obj  ./assets/obj/nanosuit/nanosuit.obj	
	
	Object* testModel = AssimpLoader::load("./assets/obj/bag/backpack.obj");
	//testModel->setScale(glm::vec3(0.05f));
	scene->addChild(testModel);
#endif

#if 0
	// 1. ����geometry
	//auto geometry = Geometry::createSphere(3.0f);
	auto geometry = Geometry::createBox(1.0f);
	Geometry* spGeometry = Geometry::createSphere(1.0f);
	Geometry* sp1Geometry = Geometry::createSphere(1.0f);

	//2. ����һ��material�������ò���
	auto material = new PhongMaterial();
	material->mShiness = 64.0f;
	material->mDiffuse = new Texture("./assets/textures/box.png", 0);
	material->mSpecularMask = new Texture("./assets/textures/sp_mask.png", 1);

	//3. ����mesh
	auto mesh = new Mesh(geometry, material);
	auto spMesh = new Mesh(spGeometry, material);
	auto sp1Mesh = new Mesh(sp1Geometry, material);
	spMesh->setPosition(glm::vec3(2.0f, 0.0f, 0.0f));
	sp1Mesh->setPosition(glm::vec3(-2.0f, 0.0f, 0.0f));

	// �������ӹ�ϵ

	scene->addChild(mesh);
	mesh->addChild(spMesh);
	mesh->addChild(sp1Mesh);
	
#endif

	// ��Դ
	//TODO: ƽ�й�
	dirLight = new DirectionalLight();
	dirLight->mSpecularIntensity = 0.1f;

	//TODO:���Դ  ����pointLights����(4�����Դ)
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

	//TODO: ������
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
	ImGui::CreateContext();		// ����imgui������
	ImGui::StyleColorsDark();	// ѡ��һ������

	// ����ImGui��GLFW��Opengl�İ�
	ImGui_ImplGlfw_InitForOpenGL(glApp->getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 460");
}

void renderIMGUI()
{
	// 1. ������ǰ��IMGUI��Ⱦ
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	

	// 2. ������ǰ��GUI��������Щ�ؼ������ϵ��£�
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

	// 3.ִ��UI��Ⱦ
	ImGui::Render();
	// ��ȡ����Ŀ���
	int display_w, display_h;
	glfwGetFramebufferSize(glApp->getWindow(), &display_w, &display_h);
	// �����ӿڴ�С
	glViewport(0, 0, display_w, display_h);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

// ����ģ�͵���ɫ��ϲ���
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

// ����ģ�Ͳ㼶������matrix���飨�õݹ飩
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

// ����ģ�Ͳ㼶������matrix���鵽mMatrixVbo
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

//	����ʵ���Ĳ���
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