#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;

out vec2 uv;
out vec3 normal;			// 法线

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;


// aPos作为attribute（属性）传入shader，原则上是不允许修改的

void main()
{
	// 将输入的顶点位置，转化为齐次坐标（三维->四维）
	vec4 transformPosition = vec4(aPos, 1.0);			

	// 做一个中间变量transformPosition，用于计算四位位置与modelMatrix相乘的中间结果	
	transformPosition = modelMatrix * transformPosition;	


	gl_Position = projectionMatrix * viewMatrix * transformPosition;

	uv = aUV;
	normal =  aNormal;
}