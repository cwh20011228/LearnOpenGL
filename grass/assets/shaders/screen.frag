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
	// ƫ������
	vec2 offsets[9]=vec2[](
		vec2(-du,dv),		// ����
		vec2(0.0f,dv),		// ����
		vec2(du,dv),		// ����
		vec2(-du,0.0f),		// ��
		vec2(0.0f,0.0f),	// ��
		vec2(du,0.0f),		// ��
		vec2(-du,-dv),		// ����
		vec2(0.0f,-dv),		// ��
		vec2(du,-dv)		// ����
	);

	// ģ��
	float kernel[9] = float[](
		1.0, 2.0, 1.0,
		2.0, 4.0, 2.0,
		1.0, 2.0, 1.0
	);

	// �񻯵ľ����
	float Sharpen[9] = float[](
		2.0, 2.0, 2.0,
		2.0, -15.0, 2.0,
		2.0, 2.0, 2.0
	);

	vec3 sumColor = vec3(0.0);
	// ��Ȩ���
	for(int i=0;i<9;i++)
	{
		vec3 samplerColor = texture(ScreenTexsampler,uv+offsets[i]).rgb;
		sumColor +=samplerColor * kernel[i];
	}

	sumColor = sumColor/16.0;

	return sumColor;
}

// ��ɫ����
vec3 colorInvert(vec3 color)
{
	vec3 InvertColor = vec3(1.0)-color;
	return InvertColor;
}

// �Ҷ�(���������Ļ����������ɫ���ϵ���ɫ����Ӱ��)
vec3 gray(vec3 color)
{
	float avg = (color.r+ color.g+color.b)/3.0;
	vec3 frayColor = vec3(avg);

	return frayColor;
}

// �������۶���ɫͨ���������У�����ɫ�����У�
// Ϊ������ȷ������Ϊÿ����ɫͨ������Ȩֵ������ɫ��һЩ����ɫ��һЩ
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