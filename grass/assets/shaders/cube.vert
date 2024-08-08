#version 460 core

layout(location = 0) in vec3 aPos;

out vec3 uvw;

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
	gl_Position = gl_Position.xyww;

	uvw = aPos;
}