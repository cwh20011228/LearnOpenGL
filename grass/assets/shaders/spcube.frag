#version 460 core
out vec4 FragColor;

in vec3 uvw;
uniform sampler2D sphericalSampler;
const float PI = 3.141592653589793238462643383279;

vec2 uvwTOuv(vec3 uvwN)
{
	float phi = asin(uvwN.y);
	float theta = atan(uvwN.z,uvwN.x);

	float u = theta/(2*PI)+0.5f;
	float v = phi/PI+0.5f;

	return vec2(u,v);
}

void main()
{
	vec3 uvwN = normalize(uvw);
	vec2 uv = uvwTOuv(uvwN);

	FragColor = texture(sphericalSampler,uv);	// 去对应圆形天空盒采样纹理
}