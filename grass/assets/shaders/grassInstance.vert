#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aColor;
layout(location = 4) in mat4 aInstanceMatrix;

out vec2 uv;
out vec3 normal;			// 法线
out vec3 worldPosition;		// 每个顶点的世界坐标
out vec2 worldXZ;

uniform mat4 modelMatrix;		// 整体调整所有instance实例的变换
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform float time;
uniform float windScale;		// 风力强度
uniform vec3 windDirection;		// 风力方向
uniform float phaseScale;		// 草的相位

// aPos作为attribute（属性）传入shader，原则上是不允许修改的

void main()
{
	// 将输入的顶点位置，转化为齐次坐标（三维->四维）
	vec4 transformPosition = vec4(aPos, 1.0);			

	// 做一个中间变量transformPosition，用于计算四位位置与modelMatrix相乘的中间结果	
	transformPosition = modelMatrix * aInstanceMatrix* transformPosition;
	
	worldXZ = transformPosition.xz;
	// 风力
	vec3 windDirN = normalize(windDirection);
	float phaseDistance = dot(windDirN,transformPosition.xyz);	// 相位
	transformPosition +=vec4((sin(time + phaseDistance / phaseScale) * (1-aColor.r) * windScale * windDirN),0.0);

	// 计算当前顶点的世界坐标位置,并且向后传输给FragmentShader
	worldPosition = transformPosition.xyz;		

	gl_Position = projectionMatrix * viewMatrix * transformPosition;

	uv = aUV;
	// modelMatrix * aInstanceMatrix 相当于实例的世界变换矩阵了
	normal = transpose(inverse(mat3(modelMatrix * aInstanceMatrix))) * aNormal;
}