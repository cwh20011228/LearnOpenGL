#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in mat4 aInstanceMatrix;

out vec2 uv;
out vec3 normal;			// ����
out vec3 worldPosition;		// ÿ���������������

uniform mat4 modelMatrix;		// �����������instanceʵ���ı任
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform mat3 normalMatrix;

// aPos��Ϊattribute�����ԣ�����shader��ԭ�����ǲ������޸ĵ�

void main()
{
	// ������Ķ���λ�ã�ת��Ϊ������꣨��ά->��ά��
	vec4 transformPosition = vec4(aPos, 1.0);			

	// ��һ���м����transformPosition�����ڼ�����λλ����modelMatrix��˵��м���	
	transformPosition = modelMatrix * aInstanceMatrix* transformPosition;	

	// ���㵱ǰ�������������λ��,����������FragmentShader
	worldPosition = transformPosition.xyz;		

	gl_Position = projectionMatrix * viewMatrix * transformPosition;

	uv = aUV;
//	normal = transpose(inverse(mat3(modelMatrix))) * aNormal;
	normal = normalMatrix * aNormal;
}