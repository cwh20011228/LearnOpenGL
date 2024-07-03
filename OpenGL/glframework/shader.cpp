#include "shader.h"
#include "../wrapper/checkError.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	// ����װ��shader�����ַ���������string
	std::string vertexCode;
	std::string fragmentCode;

	// �������ڶ�ȡvs��fs�ļ���inFileStream
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	// ��֤ifstream���������ʱ������׳��쳣
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try 
	{
		// 1. ���ļ�
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);

		// 2. ���ļ����������е��ַ������뵽stringStream����
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		// 3. �ر��ļ�
		vShaderFile.close();
		fShaderFile.close();

		// 4. ���ַ�����stringstream���ж�ȡ������ת����code string��
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR: Shader File Error :" << e.what() << std::endl;
	}

	const char* vertexShaderSource = vertexCode.c_str();
	const char* fragmentShaderSource = fragmentCode.c_str();

	// 1. ����Shader����vs��fs��
	GLuint vertex = 0, fragment = 0;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);

	// 2. ΪShader��������shader����
	glShaderSource(vertex, 1, &vertexShaderSource, NULL);
	glShaderSource(fragment, 1, &fragmentShaderSource, NULL);

	// 3. ִ��shader�������
	// 3.1 ���vertex������
	glCompileShader(vertex);
	checkShaderErrors(vertex, "COMPILE");

	// 3.2 ���fragment������
	glCompileShader(fragment);
	checkShaderErrors(fragment, "COMPILE");

	// 4. ����һ��Program����
	mProgram = glCreateProgram();

	// 5. ��vs��fs����õĽ�����������������
	glAttachShader(mProgram, vertex);
	glAttachShader(mProgram, fragment);

	// 6. ִ��program�����Ӳ����������γɿ�ִ��shader����
	glLinkProgram(mProgram);
	// ������Ӵ���
	checkShaderErrors(mProgram, "LINK");

	// 7. ����
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::~Shader()
{

}

void Shader::begin()
{
	GL_CALL(glUseProgram(mProgram));		// �󶨵�ǰ��program
}

void Shader::end()
{
	GL_CALL(glUseProgram(0));		// �����ǰ��program
}

void Shader::checkShaderErrors(GLuint target, std::string type)
{
	GLint success = 0;
	char infoLog[1024] = {};
	if (type == "COMPILE")
	{
		// ���shader������
		glGetShaderiv(target, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			glGetShaderInfoLog(target, 1024, NULL, infoLog);
			std::cout << "Error:SHADER COMPILE ERROR " << "\n" << infoLog << std::endl;
		}
	}
	else if(type=="LINK")
	{
		// ������Ӵ���
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
	// 1. ͨ�������õ�uniform������λ��location
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));

	// 2. ͨ��location����uniform������ֵ
	GL_CALL(glUniform1f(location, value));
}

void Shader::setVector3(const std::string& name, const float& x, const float& y, const float& z)
{
	// 1. ͨ�������õ�uniform������λ��location
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));

	// 2. ͨ��location����uniform������ֵ
	GL_CALL(glUniform3f(location, x, y, z));

}

void Shader::setVector3(const std::string& name, const float* values)
{
	// 1. ͨ�������õ�uniform������λ��location
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));

	// 2. ͨ��location����uniform������ֵ
	GL_CALL(glUniform3fv(location,1,values));
}

void Shader::setInt(const std::string& name, const int& value)
{
	// 1. ͨ�������õ�uniform������λ��location
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));

	// 2. ͨ��location����uniform������ֵ
	GL_CALL(glUniform1i(location, value));
}


void Shader::setMatrix4x4(const std::string& name, const glm::mat4& value)
{
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));
	glUniformMatrix4fv(location, 1, GL_FALSE,glm::value_ptr(value));
}