#pragma once

#include "core.h"
#include <string>


class Shader
{
public:
	Shader(const char* vertexPath,const char* fragmentPath);
	~Shader();

	void begin();	// ��ʼʹ�õ�ǰShader
	void end();		// ����ʹ�õ�ǰShader
	void setFloat(const std::string& name,const float& value);
	void setVector3(const std::string& name, const float& x, const float& y, const float& z);
	void setVector3(const std::string& name, const float* values);
	void setVector3(const std::string& name, const glm::vec3 value);
	void setInt(const std::string& name, const int& value);
	void setMatrix4x4(const std::string& name, const glm::mat4& value);
	void setMatrix3x3(const std::string& name, const glm::mat3& value);
	void setMatrix4x4Array(const std::string& name, const glm::mat4* value,int count);
private:
	// type : COMPLIE   LINK
	void checkShaderErrors(GLuint target,std::string type);
private:
	GLuint mProgram{ 0 };
};