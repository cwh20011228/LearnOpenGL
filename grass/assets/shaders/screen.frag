#version 460 core
out vec4 FragColor;

uniform sampler2D ScreenTexsampler;
in vec2 uv;

uniform float texWidth;
uniform float texHeight;

vec3 blur()
{
	float du = 1.0/texWidth;
	float dv = 1.0/texHeight;
	// 偏移数组
	vec2 offsets[9]=vec2[](
		vec2(-du,dv),		// 左上
		vec2(0.0f,dv),		// 正上
		vec2(du,dv),		// 右上
		vec2(-du,0.0f),		// 左
		vec2(0.0f,0.0f),	// 中
		vec2(du,0.0f),		// 右
		vec2(-du,-dv),		// 左下
		vec2(0.0f,-dv),		// 下
		vec2(du,-dv)		// 右下
	);

	// 模糊
	float kernel[9] = float[](
		1.0, 2.0, 1.0,
		2.0, 4.0, 2.0,
		1.0, 2.0, 1.0
	);

	// 锐化的卷积核
	float Sharpen[9] = float[](
		2.0, 2.0, 2.0,
		2.0, -15.0, 2.0,
		2.0, 2.0, 2.0
	);

	vec3 sumColor = vec3(0.0);
	// 加权相加
	for(int i=0;i<9;i++)
	{
		vec3 samplerColor = texture(ScreenTexsampler,uv+offsets[i]).rgb;
		sumColor +=samplerColor * kernel[i];
	}

	sumColor = sumColor/16.0;

	return sumColor;
}

// 颜色反相
vec3 colorInvert(vec3 color)
{
	vec3 InvertColor = vec3(1.0)-color;
	return InvertColor;
}

// 灰度(会对整个屏幕（包括背景色）上的颜色产生影响)
vec3 gray(vec3 color)
{
	float avg = (color.r+ color.g+color.b)/3.0;
	vec3 frayColor = vec3(avg);

	return frayColor;
}

// 由于人眼对绿色通道更加敏感，对蓝色不敏感，
// 为了物理精确，可以为每个颜色通道增加权值，让蓝色少一些，绿色多一些
vec3 grayCorrect(vec3 color)
{
	float avg = 0.2126 * color.r+0.7152 * color.g+ 0.0722 * color.b;
	vec3 frayColor = vec3(avg);

	return frayColor;
}


void main()
{
//	vec3 color = colorInvert(texture(ScreenTexsampler,uv).rgb);
//	vec3 color = grayCorrect(texture(ScreenTexsampler,uv).rgb);

	vec3 color = blur();
	FragColor = vec4(color,1.0);

//	FragColor = texture(ScreenTexsampler,uv);
}