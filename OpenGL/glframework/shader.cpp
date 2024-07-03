#include "shader.h"
#include "../wrapper/checkError.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	// 声明装入shader代码字符串的两个string
	std::string vertexCode;
	std::string fragmentCode;

	// 声明用于读取vs和fs文件的inFileStream
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	// 保证ifstream遇到问题的时候可以抛出异常
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try 
	{
		// 1. 打开文件
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);

		// 2. 将文件输入流当中的字符串输入到stringStream里面
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		// 3. 关闭文件
		vShaderFile.close();
		fShaderFile.close();

		// 4. 将字符串从stringstream当中读取出来，转化到code string中
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR: Shader File Error :" << e.what() << std::endl;
	}

	const char* vertexShaderSource = vertexCode.c_str();
	const char* fragmentShaderSource = fragmentCode.c_str();

	// 1. 创建Shader程序（vs，fs）
	GLuint vertex = 0, fragment = 0;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);

	// 2. 为Shader程序输入shader代码
	glShaderSource(vertex, 1, &vertexShaderSource, NULL);
	glShaderSource(fragment, 1, &fragmentShaderSource, NULL);

	// 3. 执行shader代码编译
	// 3.1 检查vertex编译结果
	glCompileShader(vertex);
	checkShaderErrors(vertex, "COMPILE");

	// 3.2 检查fragment编译结果
	glCompileShader(fragment);
	checkShaderErrors(fragment, "COMPILE");

	// 4. 创建一个Program壳子
	mProgram = glCreateProgram();

	// 5. 将vs与fs编译好的结果放在这个壳子里面
	glAttachShader(mProgram, vertex);
	glAttachShader(mProgram, fragment);

	// 6. 执行program的链接操作，最终形成可执行shader程序
	glLinkProgram(mProgram);
	// 检查链接错误
	checkShaderErrors(mProgram, "LINK");

	// 7. 清理
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::~Shader()
{

}

void Shader::begin()
{
	GL_CALL(glUseProgram(mProgram));		// 绑定当前的program
}

void Shader::end()
{
	GL_CALL(glUseProgram(0));		// 解除当前的program
}

void Shader::checkShaderErrors(GLuint target, std::string type)
{
	GLint success = 0;
	char infoLog[1024] = {};
	if (type == "COMPILE")
	{
		// 检查shader编译结果
		glGetShaderiv(target, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			glGetShaderInfoLog(target, 1024, NULL, infoLog);
			std::cout << "Error:SHADER COMPILE ERROR " << "\n" << infoLog << std::endl;
		}
	}
	else if(type=="LINK")
	{
		// 检查链接错误
		glGetProgramiv(target, GL_LINK_STATUS, &success);
		if (success == GL_FALSE)
		{
			glGetProgramInfoLog(target, 1024, NULL, infoLog);
			std::cout << "Error:SHADER LINK ERROR " << "\n" << infoLog << std::endl;
		}
	}
	else
	{
		std::cout << "Error:Check shader errors Type is wrong" << std::endl;
	}
}

void Shader::setFloat(const std::string& name, const float& value)
{
	// 1. 通过名称拿到uniform变量的位置location
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));

	// 2. 通过location更新uniform变量的值
	GL_CALL(glUniform1f(location, value));
}

void Shader::setVector3(const std::string& name, const float& x, const float& y, const float& z)
{
	// 1. 通过名称拿到uniform变量的位置location
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));

	// 2. 通过location更新uniform变量的值
	GL_CALL(glUniform3f(location, x, y, z));

}

void Shader::setVector3(const std::string& name, const float* values)
{
	// 1. 通过名称拿到uniform变量的位置location
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));

	// 2. 通过location更新uniform变量的值
	GL_CALL(glUniform3fv(location,1,values));
}

void Shader::setInt(const std::string& name, const int& value)
{
	// 1. 通过名称拿到uniform变量的位置location
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));

	// 2. 通过location更新uniform变量的值
	GL_CALL(glUniform1i(location, value));
}


void Shader::setMatrix4x4(const std::string& name, const glm::mat4& value)
{
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));
	glUniformMatrix4fv(location, 1, GL_FALSE,glm::value_ptr(value));
}