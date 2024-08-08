#include "renderer.hpp"

Renderer::Renderer()
{
	mPhongShader = new Shader("assets/shaders/phong.vert", "assets/shaders/phong.frag");
	mWhiteShader = new Shader("assets/shaders/white.vert", "assets/shaders/white.frag");
	mDepthShader = new Shader("assets/shaders/depth.vert", "assets/shaders/depth.frag");
	mOpacityMaskShader = new Shader("assets/shaders/phongOpacityMask.vert", "assets/shaders/phongOpacityMask.frag");
	mScreenShader = new Shader("assets/shaders/screen.vert", "assets/shaders/screen.frag");
	mCubeShader = new Shader("assets/shaders/cube.vert", "assets/shaders/cube.frag");
	mPhongEnvShader = new Shader("assets/shaders/phongEnv.vert", "assets/shaders/phongEnv.frag");
	mSpmapShader = new Shader("assets/shaders/spcube.vert", "assets/shaders/spcube.frag");
	mInstanceShader = new Shader("assets/shaders/phongInstance.vert", "assets/shaders/phongInstance.frag");
	mUInstanceShader = new Shader("assets/shaders/phongUInstance.vert", "assets/shaders/phongUInstance.frag");
	mGrassInstanceShader = new Shader("assets/shaders/grassInstance.vert", "assets/shaders/grassInstance.frag");
}

Renderer::~Renderer()
{

}

void Renderer::setClearColor(glm::vec3 color)
{
	glClearColor(color.r, color.g, color.b, 1.0);
}

Shader* Renderer::pickShader(MaterialType type)
{
	Shader* result = nullptr;
	switch (type)
	{
	case MaterialType::PhongMaterial:
		result = mPhongShader;
		break;
	case MaterialType::WhiteMaterial:
		result = mWhiteShader;
		break;
	case MaterialType::DepthMaterial:
		result = mDepthShader;
		break;
	case MaterialType::OpacityMaskMaterial:
		result = mOpacityMaskShader;
		break;
	case MaterialType::ScreenMaterial:
		result = mScreenShader;
		break;
	case MaterialType::CubeMaterial:
		result = mCubeShader;
		break;
	case MaterialType::PhongEnvMaterial:
		result = mPhongEnvShader;
		break;
	case MaterialType::SpMapMaterial:
		result = mSpmapShader;
		break;
	case MaterialType::PhongInstanceMaterial:
		result = mInstanceShader;
		break;
	case MaterialType::PhongInstanceUMaterial:
		result = mUInstanceShader;
		break;
	case MaterialType::GrassInstanceMaterial:
		result = mGrassInstanceShader;
		break;
	default:
		std::cout << "Unkonwn material type to pick shader" << std::endl;
		break;
	}

	return result;
}

void Renderer::render(
		const std::vector<Mesh*>& meshes,
		Camera* camera,
		DirectionalLight* dirLight,
		AmbientLight* ambLight
)
{
	//1. 设置当前帧绘制的时候，opengl的必要状态机参数
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//2. 清理画布
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//3. 遍历mesh进行绘制
	for (int i = 0; i < meshes.size(); i++)
	{
		auto mesh = meshes[i];
		auto geometry = mesh->mGeometry;
		auto material = mesh->mMaterial;

		// 1. 决定使用哪个shader
		Shader* shader = pickShader(material->mType);

		// 2. 更新shader的uniform
		shader->begin();	

		switch (material->mType)
		{
		case MaterialType::PhongMaterial:
		{
			PhongMaterial* phongMaterial = (PhongMaterial*)material;

			// diffuse贴图的帧更新
			GL_CALL(shader->setInt("sampler", 0));	// 采样器去0号纹理单元采样颜色像素
			// 将纹理与纹理单元挂钩
			phongMaterial->mDiffuse->bind();
			
			// 高光蒙版的帧更新
			shader->setInt("specularMaskSampler", 1);
			phongMaterial->mSpecularMask->bind();
			

			// mvp变换矩阵
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

			// 法线矩阵
			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			// 光源参数的更新
			GL_CALL(shader->setVector3("lightDirection", dirLight->mDirection));
			GL_CALL(shader->setVector3("lightColor", dirLight->mColor));
			// 光斑强度
			GL_CALL(shader->setFloat("specularIntensity", dirLight->mSpecularIntensity));
			// 光斑大小
			shader->setFloat("shiness", phongMaterial->mShiness);
			// 环境光
			GL_CALL(shader->setVector3("AmbientColor", ambLight->mColor));

			// 相机位置的更新
			GL_CALL(shader->setVector3("cameraPosition", camera->mPosition));
		}
			break;
		case MaterialType::WhiteMaterial:
		{
			// mvp变换矩阵
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));
		}
			break;
		default:
			continue;
		}

		// 3. 绑定vao
		glBindVertexArray(geometry->getVao());

		// 4. 执行绘制命令
		glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
	
		//5. 解绑当前的vao和mprogram
		glBindVertexArray(0);
		shader->end();
	}
}

void Renderer::render(
	const std::vector<Mesh*>& meshes,
	Camera* camera,
	PointLight* pointLight,
	AmbientLight* ambLight
)
{
	//1. 设置当前帧绘制的时候，opengl的必要状态机参数
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//2. 清理画布
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//3. 遍历mesh进行绘制
	for (int i = 0; i < meshes.size(); i++)
	{
		auto mesh = meshes[i];
		auto geometry = mesh->mGeometry;
		auto material = mesh->mMaterial;

		// 1. 决定使用哪个shader
		Shader* shader = pickShader(material->mType);

		// 2. 更新shader的uniform
		shader->begin();

		switch (material->mType)
		{
		case MaterialType::PhongMaterial:
		{
			PhongMaterial* phongMaterial = (PhongMaterial*)material;

			// diffuse贴图的帧更新
			GL_CALL(shader->setInt("sampler", 0));	// 采样器去0号纹理单元采样颜色像素
			// 将纹理与纹理单元挂钩
			phongMaterial->mDiffuse->bind();

			// 高光蒙版的帧更新
			shader->setInt("specularMaskSampler", 1);
			phongMaterial->mSpecularMask->bind();


			// mvp变换矩阵
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

			// 法线矩阵
			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			// 光源参数的更新
			GL_CALL(shader->setVector3("lightPosition", pointLight->getPosition()));
			GL_CALL(shader->setVector3("lightColor", pointLight->mColor));
			// 光斑强度
			GL_CALL(shader->setFloat("specularIntensity", pointLight->mSpecularIntensity));
			// 光斑大小
			shader->setFloat("shiness", phongMaterial->mShiness);
			// 环境光
			GL_CALL(shader->setVector3("AmbientColor", ambLight->mColor));
			// 点光源的衰减系数更新
			shader->setFloat("Kc", pointLight->mKc);
			shader->setFloat("K1", pointLight->mK1);
			shader->setFloat("K2", pointLight->mK2);

			// 相机位置的更新
			GL_CALL(shader->setVector3("cameraPosition", camera->mPosition));
		}
		break;
		case MaterialType::WhiteMaterial:
		{
			// mvp变换矩阵
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));
		}
		break;
		default:
			continue;
		}

		// 3. 绑定vao
		glBindVertexArray(geometry->getVao());

		// 4. 执行绘制命令
		glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);

		//5. 解绑当前的vao和mprogram
		glBindVertexArray(0);
		shader->end();
	}
}

// 聚光灯的渲染
void Renderer::render(
	const std::vector<Mesh*>& meshes,
	Camera* camera,
	DirectionalLight* dirLight,
	PointLight* pointLight,
	SpotLight* spotLight,
	AmbientLight* ambLight
)
{
	//1. 设置当前帧绘制的时候，opengl的必要状态机参数
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//2. 清理画布
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//3. 遍历mesh进行绘制
	for (int i = 0; i < meshes.size(); i++)
	{
		auto mesh = meshes[i];
		auto geometry = mesh->mGeometry;
		auto material = mesh->mMaterial;

		// 1. 决定使用哪个shader
		Shader* shader = pickShader(material->mType);

		// 2. 更新shader的uniform
		shader->begin();

		switch (material->mType)
		{
		case MaterialType::PhongMaterial:
			{
				PhongMaterial* phongMaterial = (PhongMaterial*)material;

				// diffuse贴图的帧更新
				GL_CALL(shader->setInt("sampler", 0));	// 采样器去0号纹理单元采样颜色像素
				// 将纹理与纹理单元挂钩
				phongMaterial->mDiffuse->bind();

				// 高光蒙版的帧更新
				shader->setInt("specularMaskSampler", 1);
				phongMaterial->mSpecularMask->bind();
				
				// mvp变换矩阵
				GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
				GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
				GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

				// 法线矩阵
				auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
				shader->setMatrix3x3("normalMatrix", normalMatrix);

				// 光源参数的更新

				//TODO: 聚光灯参数更新
				shader->setVector3("spotLight.position", spotLight->getPosition());
				shader->setVector3("spotLight.color", spotLight->mColor);
				shader->setVector3("spotLight.targetDirection", spotLight->mTargetDirection);
				shader->setFloat("spotLight.innerLine", glm::cos(glm::radians(spotLight->mInnerAngle)));
				shader->setFloat("spotLight.outerLine", glm::cos(glm::radians(spotLight->mOuterAngle)));
				// 光斑强度
				shader->setFloat("spotLight.specularIntensity", spotLight->mSpecularIntensity);
				
				//TODO: 平行光参数更新
				shader->setVector3("directionLight.color", dirLight->mColor);
				shader->setVector3("directionLight.direction", dirLight->mDirection);
				shader->setFloat("directionLight.specularIntensity", dirLight->mSpecularIntensity);
				
				//TODO: 点光源参数更新
				shader->setVector3("pointLight.position", pointLight->getPosition());
				shader->setVector3("pointLight.color", pointLight->mColor);
				shader->setFloat("pointLight.specularIntensity", pointLight->mSpecularIntensity);
				shader->setFloat("pointLight.k2", pointLight->mK2);
				shader->setFloat("pointLight.k1", pointLight->mK1);
				shader->setFloat("pointLight.kc", pointLight->mKc);

				// 光斑大小
				shader->setFloat("shiness", phongMaterial->mShiness);
				
				// 环境光
				GL_CALL(shader->setVector3("AmbientColor", ambLight->mColor));

				// 相机位置的更新
				GL_CALL(shader->setVector3("cameraPosition", camera->mPosition));

			}
			break;
		case MaterialType::WhiteMaterial:
			{
				// mvp变换矩阵
				GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
				GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
				GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));
				
			}
			break;
		default:
			continue;
	
		}
		// 3. 绑定vao
		glBindVertexArray(geometry->getVao());

		// 4. 执行绘制命令
		glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);

		//5. 解绑当前的vao和mprogram
		glBindVertexArray(0);
		shader->end();
	}
}


// 多光源的渲染
void Renderer::render(
	const std::vector<Mesh*>& meshes,
	Camera* camera,
	DirectionalLight* dirLight,
	const std::vector<PointLight*>& pointLights,
	SpotLight* spotLight,
	AmbientLight* ambLight
)
{
	//1. 设置当前帧绘制的时候，opengl的必要状态机参数
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//2. 清理画布
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//3. 遍历mesh进行绘制
	for (int i = 0; i < meshes.size(); i++)
	{
		auto mesh = meshes[i];
		auto geometry = mesh->mGeometry;
		auto material = mesh->mMaterial;

		// 1. 决定使用哪个shader
		Shader* shader = pickShader(material->mType);

		// 2. 更新shader的uniform
		shader->begin();

		switch (material->mType)
		{
		case MaterialType::PhongMaterial:
		{
			PhongMaterial* phongMaterial = (PhongMaterial*)material;

			// diffuse贴图的帧更新
			GL_CALL(shader->setInt("sampler", 0));	// 采样器去0号纹理单元采样颜色像素
			// 将纹理与纹理单元挂钩
			phongMaterial->mDiffuse->bind();

			// 高光蒙版的帧更新
			shader->setInt("specularMaskSampler", 1);
			phongMaterial->mSpecularMask->bind();

			// mvp变换矩阵
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

			// 法线矩阵
			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			// 光源参数的更新

			//TODO: 聚光灯参数更新
			shader->setVector3("spotLight.position", spotLight->getPosition());
			shader->setVector3("spotLight.color", spotLight->mColor);
			shader->setVector3("spotLight.targetDirection", spotLight->mTargetDirection);
			shader->setFloat("spotLight.innerLine", glm::cos(glm::radians(spotLight->mInnerAngle)));
			shader->setFloat("spotLight.outerLine", glm::cos(glm::radians(spotLight->mOuterAngle)));
			// 光斑强度
			shader->setFloat("spotLight.specularIntensity", spotLight->mSpecularIntensity);

			//TODO: 平行光参数更新
			shader->setVector3("directionLight.color", dirLight->mColor);
			shader->setVector3("directionLight.direction", dirLight->mDirection);
			shader->setFloat("directionLight.specularIntensity", dirLight->mSpecularIntensity);

			//TODO: 点光源参数更新
			for (int i = 0; i < pointLights.size(); i++)
			{
				auto pointLight = pointLights[i];
				std::string baseName = "pointLights[";
				baseName.append(std::to_string(i));
				baseName.append("]");

				shader->setVector3(baseName+".position", pointLight->getPosition());
				shader->setVector3(baseName+".color", pointLight->mColor);
				shader->setFloat(baseName+".specularIntensity", pointLight->mSpecularIntensity);
				shader->setFloat(baseName+".k2", pointLight->mK2);
				shader->setFloat(baseName+".k1", pointLight->mK1);
				shader->setFloat(baseName+".kc", pointLight->mKc);
			}

			// 光斑大小
			shader->setFloat("shiness", phongMaterial->mShiness);

			// 环境光
			GL_CALL(shader->setVector3("AmbientColor", ambLight->mColor));

			// 相机位置的更新
			GL_CALL(shader->setVector3("cameraPosition", camera->mPosition));
		}
		break;
		case MaterialType::WhiteMaterial:
		{
			// mvp变换矩阵
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));
		}
		break;
		default:
			continue;

		}
		// 3. 绑定vao
		glBindVertexArray(geometry->getVao());

		// 4. 执行绘制命令
		glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);

		//5. 解绑当前的vao和mprogram
		glBindVertexArray(0);
		shader->end();
	}
}


// 平行光的渲染
void Renderer::render(
	Scene* scene,
	Camera* camera,
	DirectionalLight* dirLight,
	AmbientLight* ambLight,
	unsigned int fbo
)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	// 开启深度检测
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);	// 开启深度写入
	// 关闭深度偏移功能
	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_POLYGON_OFFSET_LINE);
	//模板测试状态
	glEnable(GL_STENCIL_TEST);		// 开启模板测试
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);	// 设置基本写入状态
	glStencilMask(0xFF);	//  打开模板测试写入。保证了模板缓冲可以被清理
	// 默认颜色混合状态
	glDisable(GL_BLEND);
	

	//2. 清理画布
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// 清空两个队列
	mOpacityObjects.clear();
	mTransparentObjects.clear();

	// 遍历解析scene，将不透明物体和透明物体装入到不同队列中
	projectObject(scene);

	// 对透明的物体进行排序（根据相机的距离z坐标值，并且从后向前排序）
	std::sort(mTransparentObjects.begin(),
		mTransparentObjects.end(),
		[camera](const Mesh* a,const Mesh* b) ->int
		{
			auto viewMatrix = camera->getViewMatrix();
			// 计算a的相机系的z(深度)(物体初始都是放在(0,0,0)点，再通过变换到其他位置)
			auto modelMatrixA = a->getModelMatrix();
			auto worldPositionA = modelMatrixA * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			auto cameraPositionA = viewMatrix * worldPositionA;

			// 计算b的相机系的z(深度)
			auto modelMatrixB = b->getModelMatrix();
			auto worldPositionB = modelMatrixB * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			auto cameraPositionB = viewMatrix * worldPositionB;

			return cameraPositionA.z < cameraPositionB.z;	// 升序（-z轴，越小说明距离越大，先绘制）
		}
	);

	//3. 渲染两个队列
	// 渲染实体(不透明)物体队列
	for (int i = 0; i < mOpacityObjects.size(); i++)
	{
		renderObject(mOpacityObjects[i], camera, dirLight, ambLight);
	}
	// 渲染透明物体队列
	for (int i = 0; i < mTransparentObjects.size(); i++)
	{
		renderObject(mTransparentObjects[i], camera, dirLight, ambLight);
	}
	
}

// 渲染一个物体
void Renderer::renderObject(
	Object* object,
	Camera* camera,
	DirectionalLight* dirLight,
	AmbientLight* ambLight
)
{
	// 1. 判断是Mesh还是Object,如果是Mesh，需要渲染
	if (object->getType() == ObjectType::Mesh 
		|| object->getType()==ObjectType::InstanceMesh
		|| object->getType()==ObjectType::InstanceUMesh)
	{
		auto mesh = reinterpret_cast<Mesh*>(object);
		auto geometry = mesh->mGeometry;

		Material* material = nullptr;
		// 考察是否有全局材质
		if (mGlobalMaterial != nullptr)
		{
			material = mGlobalMaterial;
		}
		else
		{
			material = mesh->mMaterial;
		}
		

		// 设置当前帧绘制的时候，opengl的必要状态机参数
		
		// (1) 检测深度状态
		setDepthState(material);
		// (2) 检测偏移状态PolygonOffset
		setPolygonOffsetState(material);
		// (3) 检测模板测试状态 StencilTest
		setStencilState(material);
		// (4) 颜色混合Blend
		setBlendState(material);
		// (5) 面剔除CullFace
		setCullinghFaceState(material);


		// 1. 决定使用哪个shader
		Shader* shader = pickShader(material->mType);

		// 2. 更新shader的uniform
		shader->begin();

		switch (material->mType)
		{
		case MaterialType::PhongMaterial:
		{
			PhongMaterial* phongMaterial = (PhongMaterial*)material;

			// diffuse贴图的帧更新
			GL_CALL(shader->setInt("sampler", 0));	// 采样器去0号纹理单元采样颜色像素
			// 将纹理与纹理单元挂钩
			phongMaterial->mDiffuse->bind();

			//// 高光蒙版的帧更新
			//shader->setInt("specularMaskSampler", 1);
			//phongMaterial->mSpecularMask->bind();

			// mvp变换矩阵
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

			// 法线矩阵
			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			// 光源参数的更新
			//TODO: 平行光参数更新
			shader->setVector3("directionLight.color", dirLight->mColor);
			shader->setVector3("directionLight.direction", dirLight->mDirection);
			shader->setFloat("directionLight.specularIntensity", dirLight->mSpecularIntensity);
			shader->setFloat("directionLight.intensity", dirLight->mInstensity); // 整体光照强度更新
			
			// 光斑大小
			shader->setFloat("shiness", phongMaterial->mShiness);
			// 环境光
			GL_CALL(shader->setVector3("AmbientColor", ambLight->mColor));

			// 相机位置的更新
			GL_CALL(shader->setVector3("cameraPosition", camera->mPosition));
			// 颜色混合参数更新
			shader->setFloat("Opacity", material->mOpacity);	
		}
			break;
		case MaterialType::WhiteMaterial:
		{
			// mvp变换矩阵
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));
		}
			break;
		case MaterialType::DepthMaterial:
		{
			// mvp变换矩阵
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

			// 更新近剪裁平面和远剪裁平面距离
			shader->setFloat("near", camera->mNear);
			shader->setFloat("far", camera->mFar);
		}
			break;
		case MaterialType::OpacityMaskMaterial:
		{
			OpacityMaskMaterial* OpacityMaterial = reinterpret_cast<OpacityMaskMaterial*>(material);

			// diffuse贴图的帧更新
			GL_CALL(shader->setInt("sampler", 0));	// 采样器去0号纹理单元采样颜色像素
			// 将纹理与纹理单元挂钩
			OpacityMaterial->mDiffuse->bind();

			// opacityMask的帧更新
			shader->setInt("opacityMaskSampler", 1);
			OpacityMaterial->mOpacityMask->bind();

			// mvp变换矩阵
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

			// 法线矩阵
			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			// 光源参数的更新
			//TODO: 平行光参数更新
			shader->setVector3("directionLight.color", dirLight->mColor);
			shader->setVector3("directionLight.direction", dirLight->mDirection);
			shader->setFloat("directionLight.specularIntensity", dirLight->mSpecularIntensity);

			// 光斑大小
			shader->setFloat("shiness", OpacityMaterial->mShiness);
			// 环境光
			GL_CALL(shader->setVector3("AmbientColor", ambLight->mColor));

			// 相机位置的更新
			GL_CALL(shader->setVector3("cameraPosition", camera->mPosition));
			// 颜色混合参数更新
			shader->setFloat("Opacity", material->mOpacity);
		}
			break;
		case MaterialType::ScreenMaterial:
		{
			ScreenMaterial* screenMat = reinterpret_cast<ScreenMaterial*>(material);
			shader->setInt("ScreenTexsampler", 0);
			screenMat->mScreenTexture->bind();

			shader->setFloat("texWidth", 1920);
			shader->setFloat("texHeight", 1020);
		}
			break;
		case MaterialType::CubeMaterial:
		{
			CubeMaterial* cubeMat = reinterpret_cast<CubeMaterial*>(material);
			mesh->setPosition(camera->mPosition);
			// mvp变换矩阵
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

			shader->setInt("cubeSampler", 0);
			cubeMat->mDiffuse->bind();
		}
			break;
		case MaterialType::PhongEnvMaterial:
		{
			PhongEnvMaterial* phongEnvMaterial =reinterpret_cast<PhongEnvMaterial*>(material);

			// diffuse贴图的帧更新
			GL_CALL(shader->setInt("sampler", 0));	// 采样器去0号纹理单元采样颜色像素
			// 将纹理与纹理单元挂钩
			phongEnvMaterial->mDiffuse->bind();
			
			shader->setInt("envSampler", 1);
			phongEnvMaterial->mEnv->bind();

			// mvp变换矩阵
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

			// 法线矩阵
			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			// 光源参数的更新
			//TODO: 平行光参数更新
			shader->setVector3("directionLight.color", dirLight->mColor);
			shader->setVector3("directionLight.direction", dirLight->mDirection);
			shader->setFloat("directionLight.specularIntensity", dirLight->mSpecularIntensity);

			// 光斑大小
			shader->setFloat("shiness", phongEnvMaterial->mShiness);

			// 相机位置的更新
			GL_CALL(shader->setVector3("cameraPosition", camera->mPosition));
			// 颜色混合参数更新
			shader->setFloat("Opacity", material->mOpacity);
		}
			break;
		case MaterialType::SpMapMaterial:
		{
			SpMapMaterial* spMat = reinterpret_cast<SpMapMaterial*>(material);
			mesh->setPosition(camera->mPosition);
			// mvp变换矩阵
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

			shader->setInt("cubeSampler", 0);
			spMat->mDiffuse->bind();
		}
			break;
		case MaterialType::PhongInstanceMaterial:
		{
			PhongInstanceMaterial* InstanceMaterial = reinterpret_cast<PhongInstanceMaterial*>(material);
			InstanceMesh* instanceMesh = reinterpret_cast<InstanceMesh*>(mesh);
			
			// diffuse贴图的帧更新
			GL_CALL(shader->setInt("sampler", 0));	// 采样器去0号纹理单元采样颜色像素
			// 将纹理与纹理单元挂钩
			InstanceMaterial->mDiffuse->bind();

			//// 高光蒙版的帧更新
			//shader->setInt("specularMaskSampler", 1);
			//phongMaterial->mSpecularMask->bind();

			// mvp变换矩阵
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

			// 法线矩阵
			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			// 光源参数的更新
			//TODO: 平行光参数更新
			shader->setVector3("directionLight.color", dirLight->mColor);
			shader->setVector3("directionLight.direction", dirLight->mDirection);
			shader->setFloat("directionLight.specularIntensity", dirLight->mSpecularIntensity);

			// 光斑大小
			shader->setFloat("shiness", InstanceMaterial->mShiness);
			// 环境光
			GL_CALL(shader->setVector3("AmbientColor", ambLight->mColor));

			// 相机位置的更新
			GL_CALL(shader->setVector3("cameraPosition", camera->mPosition));
			// 颜色混合参数更新(透明度)
			shader->setFloat("Opacity", material->mOpacity);
		}
			break;
		case  MaterialType::PhongInstanceUMaterial:
		{
			PhongInstanceUMaterial* InstanceUMaterial = reinterpret_cast<PhongInstanceUMaterial*>(material);
			InstanceUMesh* UinstanceMesh = reinterpret_cast<InstanceUMesh*>(mesh);

			// diffuse贴图的帧更新
			GL_CALL(shader->setInt("sampler", 0));	// 采样器去0号纹理单元采样颜色像素
			// 将纹理与纹理单元挂钩
			InstanceUMaterial->mDiffuse->bind();

			//// 高光蒙版的帧更新
			//shader->setInt("specularMaskSampler", 1);
			//phongMaterial->mSpecularMask->bind();

			// mvp变换矩阵
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

			// 法线矩阵
			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			// 光源参数的更新
			//TODO: 平行光参数更新
			shader->setVector3("directionLight.color", dirLight->mColor);
			shader->setVector3("directionLight.direction", dirLight->mDirection);
			shader->setFloat("directionLight.specularIntensity", dirLight->mSpecularIntensity);

			// 光斑大小
			shader->setFloat("shiness", InstanceUMaterial->mShiness);
			// 环境光
			GL_CALL(shader->setVector3("AmbientColor", ambLight->mColor));

			// 相机位置的更新
			GL_CALL(shader->setVector3("cameraPosition", camera->mPosition));
			// 颜色混合参数更新(透明度)
			shader->setFloat("Opacity", material->mOpacity);

			//传输uniform类型的矩阵变换数组
			shader->setMatrix4x4Array("matrices", UinstanceMesh->mInstanceMatrices,UinstanceMesh->mInstanceCount);
		}
			break;
		case MaterialType::GrassInstanceMaterial:
		{
			GrassInstanceMaterial* grassMat = reinterpret_cast<GrassInstanceMaterial*>(material);
			InstanceMesh* instanceMesh = reinterpret_cast<InstanceMesh*>(mesh);
			instanceMesh->sortMatrices(camera->getViewMatrix());
			instanceMesh->updataMatrices();

			// diffuse贴图的帧更新
			GL_CALL(shader->setInt("sampler", 0));	// 采样器去0号纹理单元采样颜色像素
			// 将纹理与纹理单元挂钩
			grassMat->mDiffuse->bind();
			
			// 透明蒙版
			shader->setInt("opacityMask", 1);
			grassMat->mOpacityMask->bind();

			

			// mvp变换矩阵
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

			// 草地贴图UV特性
			shader->setFloat("uvScale", grassMat->mUVscale);
			shader->setFloat("brightness", grassMat->mBrightness);
			shader->setFloat("time", glfwGetTime());
			//风力方向和强度
			shader->setVector3("windDirection", grassMat->mWindDirection);
			shader->setFloat("windScale", grassMat->mWindScale);
			shader->setFloat("phaseScale", grassMat->mPhaseScale);	// 草的相位大小

			// 光源参数的更新
			//TODO: 平行光参数更新
			shader->setVector3("directionLight.color", dirLight->mColor);
			shader->setVector3("directionLight.direction", dirLight->mDirection);
			shader->setFloat("directionLight.specularIntensity", dirLight->mSpecularIntensity);

			// 光斑大小
			shader->setFloat("shiness", grassMat->mShiness);
			// 环境光
			GL_CALL(shader->setVector3("AmbientColor", ambLight->mColor));

			// 相机位置的更新
			GL_CALL(shader->setVector3("cameraPosition", camera->mPosition));
			// 颜色混合参数更新(透明度)
			shader->setFloat("Opacity", material->mOpacity);

			// 云层蒙版
			shader->setInt("cloudMask", 2);
			grassMat->mCloudMask->bind();

			shader->setVector3("cloudWhiteColor", grassMat->mCloudWhiteColor);
			shader->setVector3("cloudBlackColor", grassMat->mCloudBlackColor);
			shader->setFloat("cloudUVScale", grassMat->mCloudUVScale);
			shader->setFloat("cloudSpeed", grassMat->mCloudSpeed);
			shader->setFloat("cloudLerp", grassMat->mCloudLerp);
		}
			break;
		default:
			break;
		}

		// 3. 绑定vao
		glBindVertexArray(geometry->getVao());

		// 4. 执行绘制命令
		if (object->getType() == ObjectType::InstanceMesh)
		{
			InstanceMesh* instanceMesh = reinterpret_cast<InstanceMesh*>(mesh);
			glDrawElementsInstanced(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0, instanceMesh->mInstanceCount);
		}
		if (object->getType() == ObjectType::InstanceUMesh)
		{
			InstanceUMesh* instanceMesh = reinterpret_cast<InstanceUMesh*>(mesh);
			glDrawElementsInstanced(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0, instanceMesh->mInstanceCount);
		}
		glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);

		//5. 解绑当前的vao和mprogram
		glBindVertexArray(0);
		shader->end();
	}

	//// 2. 遍历object的子节点，对每个子节点都需要调用renderObject
	//std::vector<Object*> children = object->getChildren();
	//for (int i = 0; i < children.size(); i++)
	//{
	//	renderObject(children[i], camera, dirLight, ambLight);
	//}
}

// 设置深度测试状态
void Renderer::setDepthState(Material* material)
{
	
	if (material->mDepthTest)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(material->mDepthFunc);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	if (material->mDepthWrite)
	{
		glDepthMask(GL_TRUE);
	}
	else
	{
		glDepthMask(GL_FALSE);
	}
}

// 设置深度偏移状态
void Renderer::setPolygonOffsetState(Material* material)
{
	if (material->mPolygonOffset)
	{
		glEnable(material->mPolygonOffsetType);
		glPolygonOffset(material->mFactor, material->mUnit);

	}
	else
	{
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_POLYGON_OFFSET_LINE);
	}

}

void Renderer::setStencilState(Material* material)
{
	if (material->mStencilTest)
	{
		glEnable(GL_STENCIL_TEST);
		glStencilOp(material->mSFail, material->mZFail, material->mPass);
		glStencilMask(material->mStencilMask);
		glStencilFunc(material->mStencilFunc, material->mStencilRef, material->mStencilFuncMask);

	}
	else
	{
		glDisable(GL_STENCIL_TEST);
	}
}

// 设置颜色混合状态
void Renderer::setBlendState(Material* material)
{
	if (material->mBlend)
	{
		glEnable(GL_BLEND);
		glBlendFunc(material->mSFactor, material->mDFactor);
	}
	else
	{
		glDisable(GL_BLEND);
	}
}

void Renderer::projectObject(Object* obj)
{
	if (obj->getType() == ObjectType::Mesh 
		|| obj->getType() == ObjectType::InstanceMesh
		|| obj->getType() == ObjectType::InstanceUMesh)
	{
		Mesh* mesh = reinterpret_cast<Mesh*>(obj);
		Material* material = mesh->mMaterial;
		if (material->mBlend)
		{
			mTransparentObjects.push_back(mesh);
		}
		else
		{
			mOpacityObjects.push_back(mesh);
		}
	}
	auto children = obj->getChildren();
	for (int i = 0; i < children.size(); i++)
	{
		projectObject(children[i]);
	}
	
}

// 设置面剔除
void Renderer::setCullinghFaceState(Material* material)
{
	if (material->mFaceCulling)
	{
		glEnable(GL_CULL_FACE);		// 开启面剔除
		glFrontFace(material->mFrontFace);
		glCullFace(material->mCullFace);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
}

