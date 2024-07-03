#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;

out vec2 uv;

uniform mat4 transform;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;


// aPos作为attribute（属性）传入shader，原则上是不允许修改的
void main()
{
	vec4 position = vec4(aPos, 1.0);
	position = projectionMatrix * viewMatrix * transform * position;
	gl_Position = position;
	
	uv = vec2(aUV.xy);
}