#version 460 core

// 在shader中，各种方向向量必须单位化

out vec4 FragColor;

in vec2 uv;
in vec3 normal;
in vec3 worldPosition;		// 每个顶点的世界坐标

// diffuse贴图采样器
uniform sampler2D sampler;
//// 高光蒙版贴图采样器
//uniform sampler2D specularMaskSampler;

// 立体贴图采样器
uniform samplerCube envSampler;
	
// 相机世界位置
uniform vec3 cameraPosition;
// 光斑大小
uniform float shiness;

uniform float Opacity;		// 总体透明度


// 平行光
struct DirectionLight
{
	vec3 direction;
	vec3 color;
	float specularIntensity;	// 光照强度
};

// 点光源
struct PointLight
{
	vec3 position;
	vec3 color;
	float specularIntensity;	// 光照强度

	float k2;	// 衰减系数的二次项系数
	float k1;	// 衰减系数的一次项系数
	float kc;	// 衰减系数的常数项系数
};

// 聚光灯(	将inner和outer两个角度的cos值在cpu计算完后，传入shader	)
struct SpotLight
{
	vec3 position;				// 光源位置
	vec3 targetDirection;		// 聚光灯朝向的方向
	vec3 color;					// 光照颜色
	float outerLine;			// 外边界线角度的cos值
	float innerLine;			// 内边界线角度的cos值
	float specularIntensity;	// 光照强度
};

// 计算漫反射光照(	lightDirN,normalN是归一化后的向量		)
vec3 calculateDiffuse(vec3 lightColor,vec3 objectColor,vec3 lightDirN,vec3 normalN)
{
	float diffuse = clamp(dot(-lightDirN, normalN),0.0,1.0);	// 法线与光源入射方向夹角的cos值
	vec3 diffuseColor = lightColor * diffuse * objectColor;

	return diffuseColor;
}

// 计算specular(镜面反射)的光照
vec3 calculateSpecular(vec3 lightColor,vec3 lightDirN,vec3 normalN,vec3 viewDirN,float intensity)
{
	float dotResult = dot(-lightDirN, normalN);		// 防止背面光效果(入射光与法线夹角)
	//  dotResult>0 ,正面照射，不剔除； dotResult<0,背面照射，剔除；
	float flag = step(0.0, dotResult);		

	vec3 lightReflect = normalize(reflect(lightDirN, normalN));		//反射光
	
	float specular =max(dot(lightReflect, -viewDirN), 0.0);			// 控制光斑大小
	specular = pow(specular,shiness);
//	// 高光蒙版贴图的r值（权重）
//	float specularMask = texture(specularMaskSampler,uv).r;

	vec3  specularColor = lightColor * specular * flag * intensity  ;

	return specularColor;

}

// 计算聚光灯光照
vec3 calculateSpotLight(SpotLight light,vec3 normalN,vec3 viewDirN)
{
	// 计算光照的通用数据
	vec3 objectColor = texture(sampler, uv).xyz;				// 获取物体的当前像素的颜色
	vec3 lightDirN = normalize(worldPosition-light.position);	// 入射光的方向
	vec3 targetDirectionN = normalize(light.targetDirection);	// 聚光灯的朝向方向

	// 计算SpotLight聚光的照射范围
	float cGamma = dot(targetDirectionN,lightDirN);
	float intensity = clamp((cGamma-light.outerLine)/(light.innerLine-light.outerLine),0.0,1.0);

	vec3 diffuseColor = calculateDiffuse(light.color,objectColor,lightDirN,normalN);
	vec3 specularColor = calculateSpecular(light.color,lightDirN,normalN,viewDirN,light.specularIntensity);
	
	return (diffuseColor + specularColor) * intensity;
}

// 计算平行光的光照
vec3 calculateDirectionalLight(DirectionLight light,vec3 normalN,vec3 viewDirN)
{
	vec3 objectColor = texture(sampler,uv).xyz;
	vec3 lightDirN = normalize(light.direction);

	vec3 diffuseColor = calculateDiffuse(light.color,objectColor,lightDirN,normalN);
	vec3 specularColor = calculateSpecular(light.color,lightDirN,normalN,viewDirN,light.specularIntensity);
	return diffuseColor + specularColor;
}

// 计算点光源的光照
vec3 calculatePointLight(PointLight light,vec3 normalN,vec3 viewDirN)
{
	// 计算光照的通用数据
	vec3 objectColor = texture(sampler, uv).xyz;				// 获取物体的当前像素的颜色
	vec3 lightDirection = worldPosition-light.position;
	vec3 lightDirN = normalize(lightDirection);	// 入射光的方向

	// 计算衰减值
	float dist = length(lightDirection);	// 光源和像素之间的距离
	float attenuation = 1.0 / (light.kc+light.k1*dist +light.k2*dist *dist );	// 衰减权重

	vec3 diffuseColor = calculateDiffuse(light.color,objectColor,lightDirN,normalN);
	vec3 specularColor = calculateSpecular(light.color,lightDirN,normalN,viewDirN,light.specularIntensity);
	
	return ( diffuseColor + specularColor ) * attenuation;
}

// 计算立体贴图作为环境的环境映射
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

	// 计算光照的通用数据
	vec3 objectColor = texture(sampler, uv).xyz;		// 获取物体的当前像素的颜色
	float alpha = texture(sampler,uv).a;				// 贴图的透明度	
	vec3 viewDirN = normalize(worldPosition - cameraPosition);  // 视线方向向量
	vec3 normalN = normalize(normal);			// 将vs中输入的normal做一下归一化
	vec3 lightDirN = normalize(worldPosition-spotLight.position);
	vec3 targetDirN = normalize(spotLight.targetDirection);	

	result += calculateDirectionalLight(directionLight, normalN, viewDirN);	  // 平行光
	// 环境光计算
	vec3 ambientColor = objectColor * calculateEnv(normalN,viewDirN);

	vec3 finalColor = result + ambientColor;

	alpha = Opacity * alpha;

	FragColor = vec4(finalColor, alpha);
}