#include "application.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// ��̬���������ݶδ洢���ڱ���ʱ�����ڴ棬������ʱ���״�ʹ��ʱ��ʼ��
Application* Application::mInstance = nullptr;

Application* Application::getInstance()
{
	if (mInstance == nullptr)
	{
		mInstance = new Application();
	}
	return mInstance;
}
 

Application::Application()
{
	
}

Application::~Application()
{
	/*if (mInstance != nullptr)
	{
		delete mInstance;
		mInstance = nullptr;
	}*/
}

bool Application::Init(const int& width, const int& height, const char* title)
{
	m_width = width;
	m_height = height;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	mWindow = glfwCreateWindow(m_width,m_height, title, NULL, NULL);
	if (mWindow == nullptr)
	{
		return false;
	}
	glfwMakeContextCurrent(mWindow);

	// ʹ��glad�������а汾opengl����
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GlAD!" << std::endl;

		return false;
	}

	// �� frameBufferSizeCallBack ���뵽glfw������ ,��������仯����Ϣ
	glfwSetFramebufferSizeCallback(mWindow, frameBufferSizeCallBack);
	glfwSetKeyCallback(mWindow, keyCallback);			// ��������
	glfwSetMouseButtonCallback(mWindow, MouseCallback);	// �������	����¼���Ӧ
	glfwSetCursorPosCallback(mWindow, cursorCallback);	// �������λ�� �ƶ��¼���Ӧ
	glfwSetScrollCallback(mWindow, scrollCallback);		// ����������

	// thisָ��ָ��ǰȫ��Ψһ��Application����
	glfwSetWindowUserPointer(mWindow, this);

	return true;
}

bool Application::update()
{
	if (glfwWindowShouldClose(mWindow))
	{
		return false;
	}
	glfwPollEvents();   // ���ղ��ַ�������Ϣ

	glfwSwapBuffers(mWindow);	// �л�˫����

	return true;
}

void Application::destory()
{
	glfwTerminate();
}

void Application::setResizeCallback(ResizeCallBack callback)
{
	mResizeCallBack = callback;
}

void Application::setKeyBoardCallback(KeyBoardCallBack callback)
{
	mKeyBoardCallBack = callback;
}


void Application::frameBufferSizeCallBack(	GLFWwindow* window,
											int width,
											int height)
{
	Application* self = (Application*)glfwGetWindowUserPointer(window);
	if (self->mResizeCallBack != nullptr)
	{
		self->mResizeCallBack(width, height);
	}
	/*if (Application::getInstance()->mResizeCallBack != nullptr)
	{
		Application::getInstance()->mResizeCallBack(width, height);
	}*/
	std::cout << "mResizeCallBack" << std::endl;
}


// ������Ϣ�ص�����
void Application::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Application* self = (Application*)glfwGetWindowUserPointer(window);
	if (self->mKeyBoardCallBack != nullptr)
	{
		self->mKeyBoardCallBack(key,action,mods);
	}
}

void Application::MouseCallback(GLFWwindow* window,
								int button,
								int action,
								int mods)
{
	Application* self = (Application*)glfwGetWindowUserPointer(window);
	if (self->mMouseCallBack != nullptr)
	{
		self->mMouseCallBack(button, action, mods);
	}
}


void Application::cursorCallback(GLFWwindow* window,
								double xpos,
								double ypos)
{
	Application* self = (Application*)glfwGetWindowUserPointer(window);
	if (self->mCursorCallBack != nullptr)
	{
		self->mCursorCallBack(xpos,ypos);
	}
}

void Application::scrollCallback(GLFWwindow* window,
								double xoffset,
								double yoffset)
{
	Application* self = (Application*)glfwGetWindowUserPointer(window);
	if (self->mSrollCallBack != nullptr)
	{
		self->mSrollCallBack(yoffset);
	}
}


void Application::setMouseCallBack(MouseCallBack callback)
{
	mMouseCallBack = callback;
}

void Application::setCursorCallBack(CursorCallBack callback)
{
	mCursorCallBack = callback;
}

void Application::getCursorPosition(double* x, double* y)
{
	glfwGetCursorPos(mWindow, x, y);
}

void Application::setSrollCallBack(SrollCallBack callback)
{
	mSrollCallBack = callback;
}

//GLFWwindow* Application::getWindow() const{
//	return mWindow; // ����������Ĵ���ָ��
//}