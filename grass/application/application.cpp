#include "application.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>


// 静态变量在数据段存储，在编译时分配内存，（运行时）首次使用时初始化
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

bool Application::Init(const int& width, const int& height, std::string title)
{
	m_width = width;
	m_height = height;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	mWindow = glfwCreateWindow(m_width, m_height, title.c_str(), NULL, NULL);
	if (mWindow == nullptr)
	{
		return false;
	}
	glfwMakeContextCurrent(mWindow);

	// 使用glad加载所有版本opengl函数
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GlAD!" << std::endl;

		return false;
	}

	// 将 frameBufferSizeCallBack 加入到glfw监听中 ,监听窗体变化的消息
	glfwSetFramebufferSizeCallback(mWindow, frameBufferSizeCallBack);
	glfwSetKeyCallback(mWindow, keyCallback);			// 监听键盘
	glfwSetMouseButtonCallback(mWindow, MouseCallback);	// 监听鼠标	点击事件响应
	glfwSetCursorPosCallback(mWindow, cursorCallback);	// 监听鼠标位置 移动事件响应
	glfwSetScrollCallback(mWindow, scrollCallback);		// 监听鼠标滚轮

	// this指针指向当前全局唯一的Application对象
	glfwSetWindowUserPointer(mWindow, this);

	return true;
}

bool Application::update()
{
	// 启用V-Sync(启用垂直同步)
	//glfwSwapInterval(1);

	if (!glfwWindowShouldClose(mWindow))
	{
		glfwPollEvents();   // 接收并分发窗口信息

		glfwSwapBuffers(mWindow);	// 切换双缓存

		return true;
	}

	return false;
}

void Application::calculateFPS(double& preTime, unsigned int& counter)
{
	double timeDiff;

	double crntTime = glfwGetTime();
	timeDiff = crntTime - preTime;
	counter++;

	// 每秒更新一次
	if (timeDiff >= 1.0)
	{
		std::string FPS = std::to_string((1.0 / timeDiff) * counter);
		std::string ms = std::to_string((timeDiff / counter) * 1000);
		std::string fps = "OpenglGrass: " + FPS + " FPS / " + ms + "ms";

		glfwSetWindowTitle(mWindow, fps.c_str());

		preTime = crntTime;
		counter = 0;
	}
}

void Application::setTitle(const std::string& title)
{
	glfwSetWindowTitle(mWindow, title.c_str());
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


void Application::frameBufferSizeCallBack(GLFWwindow* window,
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


// 键盘消息回调函数
void Application::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Application* self = (Application*)glfwGetWindowUserPointer(window);
	if (self->mKeyBoardCallBack != nullptr)
	{
		self->mKeyBoardCallBack(key, action, mods);
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
		self->mCursorCallBack(xpos, ypos);
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
//	return mWindow; // 假设这是你的窗口指针
//}