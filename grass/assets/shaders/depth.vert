#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;

out vec2 uv;
out vec3 normal;			// ����

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;


// aPos��Ϊattribute�����ԣ�����shader��ԭ�����ǲ������޸ĵ�

void main()
{
	// ������Ķ���λ�ã�ת��Ϊ������꣨��ά->��ά��
	vec4 transformPosition = vec4(aPos, 1.0);			

	// ��һ���м����transformPosition�����ڼ�����λλ����modelMatrix��˵��м���	
	transformPosition = modelMatrix * transformPosition;	


	gl_Position = projectionMatrix * viewMatrix * transformPosition;

	uv = aUV;
	normal =  aNormal;
}