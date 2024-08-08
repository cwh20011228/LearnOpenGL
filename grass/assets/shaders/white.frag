#version 460 core

// 在shader中，各种方向向量必须单位化

out vec4 FragColor;

in vec2 uv;
in vec3 normal;

void main()
{
	FragColor = vec4(vec3(1.0), 1.0);
}