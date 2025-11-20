#pragma once

#include <iostream>
#include "../../glframework/core.h"

#define glApp Application::getInstance()

class GLFWwindow;
using ResizeCallBack = void(*)(int width, int height);
using KeyBoardCallBack = void(*)(int key, int action, int mods);
using MouseCallBack = void(*)(int button, int action, int mods);
using CursorCallBack = void(*)(double xpos, double ypos);
using SrollCallBack = void(*)(double offset);


class Application
{
public:
	static Application* getInstance();
	uint32_t getWidth() const { return m_width; }
	uint32_t getHeight() const { return m_height; }
	void getCursorPosition(double* x, double* y);			// 获取鼠标位置
	bool Init(const int& width = 800, const int& height = 600, std::string title = "OpenGL_Grass");
	bool update();
	void destory();
	void calculateFPS(double& preTime, unsigned int& counter);
	void setTitle(const std::string& title);

	void setResizeCallback(ResizeCallBack callback);
	void setKeyBoardCallback(KeyBoardCallBack callback);
	void setMouseCallBack(MouseCallBack callback);
	void setCursorCallBack(CursorCallBack callback);
	void setSrollCallBack(SrollCallBack callback);

	GLFWwindow* getWindow() const { return mWindow; };

	~Application();
private:
	static void frameBufferSizeCallBack(GLFWwindow* window,
		int width,
		int height);
	static void keyCallback(GLFWwindow* window,
		int key,
		int scancode,
		int action,
		int mods);
	static void MouseCallback(GLFWwindow* window,
		int button,
		int action,
		int mods);
	static void cursorCallback(GLFWwindow* window,
		double xpos,
		double ypos);
	static void scrollCallback(GLFWwindow* window,
		double xoffset,
		double yoffset);

private:
	Application();
	Application(const Application&) = delete;
	Application operator=(const Application&) = delete;
private:
	static Application* mInstance;

	uint32_t m_width{ 0 };
	uint32_t m_height{ 0 };
	GLFWwindow* mWindow{ nullptr };

	ResizeCallBack mResizeCallBack{ nullptr };
	KeyBoardCallBack mKeyBoardCallBack{ nullptr };
	MouseCallBack mMouseCallBack{ nullptr };
	CursorCallBack mCursorCallBack{ nullptr };
	SrollCallBack mSrollCallBack{ nullptr };
};

