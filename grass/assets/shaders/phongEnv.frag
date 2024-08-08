#version 460 core

// ��shader�У����ַ����������뵥λ��

out vec4 FragColor;

in vec2 uv;
in vec3 normal;
in vec3 worldPosition;		// ÿ���������������

// diffuse��ͼ������
uniform sampler2D sampler;
//// �߹��ɰ���ͼ������
//uniform sampler2D specularMaskSampler;

// ������ͼ������
uniform samplerCube envSampler;
	
// �������λ��
uniform vec3 cameraPosition;
// ��ߴ�С
uniform float shiness;

uniform float Opacity;		// ����͸����


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
//	// �߹��ɰ���ͼ��rֵ��Ȩ�أ�
//	float specularMask = texture(specularMaskSampler,uv).r;

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
vec3 calculateDirectionalLight(DirectionLight light,vec3 normalN,vec3 viewDirN)
{
	vec3 objectColor = texture(sampler,uv).xyz;
	vec3 lightDirN = normalize(light.direction);

	vec3 diffuseColor = calculateDiffuse(light.color,objectColor,lightDirN,normalN);
	vec3 specularColor = calculateSpecular(light.color,lightDirN,normalN,viewDirN,light.specularIntensity);
	return diffuseColor + specularColor;
}

// ������Դ�Ĺ���
vec3 calculatePointLight(PointLight light,vec3 normalN,vec3 viewDirN)
{
	// ������յ�ͨ������
	vec3 objectColor = texture(sampler, uv).xyz;				// ��ȡ����ĵ�ǰ���ص���ɫ
	vec3 lightDirection = worldPosition-light.position;
	vec3 lightDirN = normalize(lightDirection);	// �����ķ���

	// ����˥��ֵ
	float dist = length(lightDirection);	// ��Դ������֮��ľ���
	float attenuation = 1.0 / (light.kc+light.k1*dist +light.k2*dist *dist );	// ˥��Ȩ��

	vec3 diffuseColor = calculateDiffuse(light.color,objectColor,lightDirN,normalN);
	vec3 specularColor = calculateSpecular(light.color,lightDirN,normalN,viewDirN,light.specularIntensity);
	
	return ( diffuseColor + specularColor ) * attenuation;
}

// ����������ͼ��Ϊ�����Ļ���ӳ��
vec3 calculateEnv(vec3 normal,vec3 viewDir)
{
	vec3 reflectDir=normalize(reflect(viewDir,normal));
	vec3 color = texture(envSampler,reflectDir).xyz;

	return color;
}
uniform SpotLight spotLight;
uniform DirectionLight directionLight;


void main()
{
	vec3 result = vec3(0.0,0.0,0.0);

	// ������յ�ͨ������
	vec3 objectColor = texture(sampler, uv).xyz;		// ��ȡ����ĵ�ǰ���ص���ɫ
	float alpha = texture(sampler,uv).a;				// ��ͼ��͸����	
	vec3 viewDirN = normalize(worldPosition - cameraPosition);  // ���߷�������
	vec3 normalN = normalize(normal);			// ��vs�������normal��һ�¹�һ��
	vec3 lightDirN = normalize(worldPosition-spotLight.position);
	vec3 targetDirN = normalize(spotLight.targetDirection);	

	result += calculateDirectionalLight(directionLight, normalN, viewDirN);	  // ƽ�й�
	// ���������
	vec3 ambientColor = objectColor * calculateEnv(normalN,viewDirN);

	vec3 finalColor = result + ambientColor;

	alpha = Opacity * alpha;

	FragColor = vec4(finalColor, alpha);
}