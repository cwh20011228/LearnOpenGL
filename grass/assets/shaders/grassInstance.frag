#version 460 core

// ��shader�У����ַ����������뵥λ��

out vec4 FragColor;

in vec2 uv;
in vec3 normal;
in vec3 worldPosition;		// ÿ���������������
in vec2 worldXZ;

// diffuse��ͼ������
uniform sampler2D sampler;
// ͸���ɰ�
uniform sampler2D opacityMask;
// �ƶ��ɰ�
uniform sampler2D cloudMask;

//������
uniform vec3 AmbientColor;		
// �������λ��
uniform vec3 cameraPosition;
// ��ߴ�С
uniform float shiness;

uniform float Opacity;		// ����͸����
// �ݵ���ͼ����
uniform float uvScale;
uniform float brightness;	// ����

// �Ʋ��ɫ�ͺ�ɫ������ɫ
uniform vec3 cloudWhiteColor;
uniform vec3 cloudBlackColor;
uniform float cloudUVScale;
uniform float cloudSpeed;
uniform float cloudLerp;		// �Ʋ��Ȩ��
uniform float time;				// ��vert��frag�ж������ˣ�ֻ��Ҫrender�и���һ��time����
uniform vec3 windDirection;		// ��������


// ƽ�й�
struct DirectionLight
{
	vec3 direction;
	vec3 color;
	float specularIntensity;	// ����ǿ��
};

// ���Դ
struct PointLight
{
	vec3 position;
	vec3 color;
	float specularIntensity;	// ����ǿ��

	float k2;	// ˥��ϵ���Ķ�����ϵ��
	float k1;	// ˥��ϵ����һ����ϵ��
	float kc;	// ˥��ϵ���ĳ�����ϵ��
};

// �۹��(	��inner��outer�����Ƕȵ�cosֵ��cpu������󣬴���shader	)
struct SpotLight
{
	vec3 position;				// ��Դλ��
	vec3 targetDirection;		// �۹�Ƴ���ķ���
	vec3 color;					// ������ɫ
	float outerLine;			// ��߽��߽Ƕȵ�cosֵ
	float innerLine;			// �ڱ߽��߽Ƕȵ�cosֵ
	float specularIntensity;	// ����ǿ��
};

// �������������(	lightDirN,normalN�ǹ�һ���������		)
vec3 calculateDiffuse(vec3 lightColor,vec3 objectColor,vec3 lightDirN,vec3 normalN)
{
	float diffuse = clamp(dot(-lightDirN, normalN),0.0,1.0);	// �������Դ���䷽��нǵ�cosֵ
	vec3 diffuseColor = lightColor * diffuse * objectColor;

	return diffuseColor;
}

// ����specular(���淴��)�Ĺ���
vec3 calculateSpecular(vec3 lightColor,vec3 lightDirN,vec3 normalN,vec3 viewDirN,float intensity)
{
	float dotResult = dot(-lightDirN, normalN);		// ��ֹ�����Ч��(������뷨�߼н�)
	//  dotResult>0 ,�������䣬���޳��� dotResult<0,�������䣬�޳���
	float flag = step(0.0, dotResult);		

	vec3 lightReflect = normalize(reflect(lightDirN, normalN));		//�����
	
	float specular =max(dot(lightReflect, -viewDirN), 0.0);			// ���ƹ�ߴ�С
	specular = pow(specular,shiness);

	vec3  specularColor = lightColor * specular * flag * intensity  ;

	return specularColor;

}

// ����۹�ƹ���
vec3 calculateSpotLight(SpotLight light,vec3 normalN,vec3 viewDirN)
{
	// ������յ�ͨ������
	vec3 objectColor = texture(sampler, uv).xyz;				// ��ȡ����ĵ�ǰ���ص���ɫ
	vec3 lightDirN = normalize(worldPosition-light.position);	// �����ķ���
	vec3 targetDirectionN = normalize(light.targetDirection);	// �۹�Ƶĳ�����

	// ����SpotLight�۹�����䷶Χ
	float cGamma = dot(targetDirectionN,lightDirN);
	float intensity = clamp((cGamma-light.outerLine)/(light.innerLine-light.outerLine),0.0,1.0);

	vec3 diffuseColor = calculateDiffuse(light.color,objectColor,lightDirN,normalN);
	vec3 specularColor = calculateSpecular(light.color,lightDirN,normalN,viewDirN,light.specularIntensity);
	
	return (diffuseColor + specularColor) * intensity;
}

// ����ƽ�й�Ĺ���
vec3 calculateDirectionalLight(vec3 objectColor,DirectionLight light,vec3 normalN,vec3 viewDirN)
{
	vec3 lightDirN = normalize(light.direction);

	vec3 diffuseColor = calculateDiffuse(light.color,objectColor,lightDirN,normalN);
	vec3 specularColor = calculateSpecular(light.color,lightDirN,normalN,viewDirN,light.specularIntensity);
	return diffuseColor + specularColor;
}

// ������Դ�Ĺ���
vec3 calculatePointLight(vec3 objectColor,PointLight light,vec3 normalN,vec3 viewDirN)
{
	// ������յ�ͨ������
	vec3 lightDirection = worldPosition-light.position;
	vec3 lightDirN = normalize(lightDirection);	// �����ķ���

	// ����˥��ֵ
	float dist = length(lightDirection);	// ��Դ������֮��ľ���
	float attenuation = 1.0 / (light.kc+light.k1*dist +light.k2*dist *dist );	// ˥��Ȩ��

	vec3 diffuseColor = calculateDiffuse(light.color,objectColor,lightDirN,normalN);
	vec3 specularColor = calculateSpecular(light.color,lightDirN,normalN,viewDirN,light.specularIntensity);
	
	return ( diffuseColor + specularColor ) * attenuation;
}

uniform SpotLight spotLight;
uniform DirectionLight directionLight;
#define POINT_LIGHT_NUM 4
uniform PointLight pointLights[4];

void main()
{
	vec3 result = vec3(0.0,0.0,0.0);

	// ������յ�ͨ������
	vec2 worldUV = worldXZ/uvScale;
	vec3 objectColor = texture(sampler,worldUV).xyz * brightness;		// ��ȡ����ĵ�ǰ���ص���ɫ
	float alpha = texture(opacityMask,uv).r;				// ͸���ɰ��͸����	
	vec3 viewDirN = normalize(worldPosition - cameraPosition);  // ���߷�������
	vec3 normalN = normalize(normal);			// ��vs�������normal��һ�¹�һ��
	vec3 lightDirN = normalize(worldPosition-spotLight.position);
	vec3 targetDirN = normalize(spotLight.targetDirection);	

//	result += calculateSpotLight(spotLight, normalN, viewDirN);		// �۹��
	result += calculateDirectionalLight(objectColor,directionLight, normalN, viewDirN);	  // ƽ�й�
//	for(int i=0;i<POINT_LIGHT_NUM;i++)
//	{
//		result += calculatePointLight(objectColor,pointLights[i], normalN, viewDirN);	// ���Դ
//	}

	// ���������
	vec3 ambientColor = objectColor * AmbientColor;

	vec3 grassColor = result + ambientColor;

	alpha = Opacity * alpha;

	vec3 windDirN = normalize(windDirection);
	vec2 coludUV = worldXZ / cloudUVScale;
	coludUV = coludUV+ time * cloudSpeed * windDirN.xz;

	float mCloudMask = texture(cloudMask,coludUV).r;
	// mix���Բ�ֵ��cloudBlackColor * (1-mCloudMask) + cloudWhiteColor * mCloudMask
	// ƽ���ع�����ɫ
	vec3 cloudColor = mix(cloudBlackColor,cloudWhiteColor,mCloudMask);

	vec3 finalColor = mix(grassColor,cloudColor,cloudLerp);
	FragColor = vec4(finalColor,alpha);
}