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
	//1. ���õ�ǰ֡���Ƶ�ʱ��opengl�ı�Ҫ״̬������
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//2. ������
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//3. ����mesh���л���
	for (int i = 0; i < meshes.size(); i++)
	{
		auto mesh = meshes[i];
		auto geometry = mesh->mGeometry;
		auto material = mesh->mMaterial;

		// 1. ����ʹ���ĸ�shader
		Shader* shader = pickShader(material->mType);

		// 2. ����shader��uniform
		shader->begin();	

		switch (material->mType)
		{
		case MaterialType::PhongMaterial:
		{
			PhongMaterial* phongMaterial = (PhongMaterial*)material;

			// diffuse��ͼ��֡����
			GL_CALL(shader->setInt("sampler", 0));	// ������ȥ0������Ԫ������ɫ����
			// ������������Ԫ�ҹ�
			phongMaterial->mDiffuse->bind();
			
			// �߹��ɰ��֡����
			shader->setInt("specularMaskSampler", 1);
			phongMaterial->mSpecularMask->bind();
			

			// mvp�任����
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

			// ���߾���
			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			// ��Դ�����ĸ���
			GL_CALL(shader->setVector3("lightDirection", dirLight->mDirection));
			GL_CALL(shader->setVector3("lightColor", dirLight->mColor));
			// ���ǿ��
			GL_CALL(shader->setFloat("specularIntensity", dirLight->mSpecularIntensity));
			// ��ߴ�С
			shader->setFloat("shiness", phongMaterial->mShiness);
			// ������
			GL_CALL(shader->setVector3("AmbientColor", ambLight->mColor));

			// ���λ�õĸ���
			GL_CALL(shader->setVector3("cameraPosition", camera->mPosition));
		}
			break;
		case MaterialType::WhiteMaterial:
		{
			// mvp�任����
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));
		}
			break;
		default:
			continue;
		}

		// 3. ��vao
		glBindVertexArray(geometry->getVao());

		// 4. ִ�л�������
		glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
	
		//5. ���ǰ��vao��mprogram
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
	//1. ���õ�ǰ֡���Ƶ�ʱ��opengl�ı�Ҫ״̬������
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//2. ������
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//3. ����mesh���л���
	for (int i = 0; i < meshes.size(); i++)
	{
		auto mesh = meshes[i];
		auto geometry = mesh->mGeometry;
		auto material = mesh->mMaterial;

		// 1. ����ʹ���ĸ�shader
		Shader* shader = pickShader(material->mType);

		// 2. ����shader��uniform
		shader->begin();

		switch (material->mType)
		{
		case MaterialType::PhongMaterial:
		{
			PhongMaterial* phongMaterial = (PhongMaterial*)material;

			// diffuse��ͼ��֡����
			GL_CALL(shader->setInt("sampler", 0));	// ������ȥ0������Ԫ������ɫ����
			// ������������Ԫ�ҹ�
			phongMaterial->mDiffuse->bind();

			// �߹��ɰ��֡����
			shader->setInt("specularMaskSampler", 1);
			phongMaterial->mSpecularMask->bind();


			// mvp�任����
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

			// ���߾���
			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			// ��Դ�����ĸ���
			GL_CALL(shader->setVector3("lightPosition", pointLight->getPosition()));
			GL_CALL(shader->setVector3("lightColor", pointLight->mColor));
			// ���ǿ��
			GL_CALL(shader->setFloat("specularIntensity", pointLight->mSpecularIntensity));
			// ��ߴ�С
			shader->setFloat("shiness", phongMaterial->mShiness);
			// ������
			GL_CALL(shader->setVector3("AmbientColor", ambLight->mColor));
			// ���Դ��˥��ϵ������
			shader->setFloat("Kc", pointLight->mKc);
			shader->setFloat("K1", pointLight->mK1);
			shader->setFloat("K2", pointLight->mK2);

			// ���λ�õĸ���
			GL_CALL(shader->setVector3("cameraPosition", camera->mPosition));
		}
		break;
		case MaterialType::WhiteMaterial:
		{
			// mvp�任����
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));
		}
		break;
		default:
			continue;
		}

		// 3. ��vao
		glBindVertexArray(geometry->getVao());

		// 4. ִ�л�������
		glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);

		//5. ���ǰ��vao��mprogram
		glBindVertexArray(0);
		shader->end();
	}
}

// �۹�Ƶ���Ⱦ
void Renderer::render(
	const std::vector<Mesh*>& meshes,
	Camera* camera,
	DirectionalLight* dirLight,
	PointLight* pointLight,
	SpotLight* spotLight,
	AmbientLight* ambLight
)
{
	//1. ���õ�ǰ֡���Ƶ�ʱ��opengl�ı�Ҫ״̬������
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//2. ������
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//3. ����mesh���л���
	for (int i = 0; i < meshes.size(); i++)
	{
		auto mesh = meshes[i];
		auto geometry = mesh->mGeometry;
		auto material = mesh->mMaterial;

		// 1. ����ʹ���ĸ�shader
		Shader* shader = pickShader(material->mType);

		// 2. ����shader��uniform
		shader->begin();

		switch (material->mType)
		{
		case MaterialType::PhongMaterial:
			{
				PhongMaterial* phongMaterial = (PhongMaterial*)material;

				// diffuse��ͼ��֡����
				GL_CALL(shader->setInt("sampler", 0));	// ������ȥ0������Ԫ������ɫ����
				// ������������Ԫ�ҹ�
				phongMaterial->mDiffuse->bind();

				// �߹��ɰ��֡����
				shader->setInt("specularMaskSampler", 1);
				phongMaterial->mSpecularMask->bind();
				
				// mvp�任����
				GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
				GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
				GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

				// ���߾���
				auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
				shader->setMatrix3x3("normalMatrix", normalMatrix);

				// ��Դ�����ĸ���

				//TODO: �۹�Ʋ�������
				shader->setVector3("spotLight.position", spotLight->getPosition());
				shader->setVector3("spotLight.color", spotLight->mColor);
				shader->setVector3("spotLight.targetDirection", spotLight->mTargetDirection);
				shader->setFloat("spotLight.innerLine", glm::cos(glm::radians(spotLight->mInnerAngle)));
				shader->setFloat("spotLight.outerLine", glm::cos(glm::radians(spotLight->mOuterAngle)));
				// ���ǿ��
				shader->setFloat("spotLight.specularIntensity", spotLight->mSpecularIntensity);
				
				//TODO: ƽ�й��������
				shader->setVector3("directionLight.color", dirLight->mColor);
				shader->setVector3("directionLight.direction", dirLight->mDirection);
				shader->setFloat("directionLight.specularIntensity", dirLight->mSpecularIntensity);
				
				//TODO: ���Դ��������
				shader->setVector3("pointLight.position", pointLight->getPosition());
				shader->setVector3("pointLight.color", pointLight->mColor);
				shader->setFloat("pointLight.specularIntensity", pointLight->mSpecularIntensity);
				shader->setFloat("pointLight.k2", pointLight->mK2);
				shader->setFloat("pointLight.k1", pointLight->mK1);
				shader->setFloat("pointLight.kc", pointLight->mKc);

				// ��ߴ�С
				shader->setFloat("shiness", phongMaterial->mShiness);
				
				// ������
				GL_CALL(shader->setVector3("AmbientColor", ambLight->mColor));

				// ���λ�õĸ���
				GL_CALL(shader->setVector3("cameraPosition", camera->mPosition));

			}
			break;
		case MaterialType::WhiteMaterial:
			{
				// mvp�任����
				GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
				GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
				GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));
				
			}
			break;
		default:
			continue;
	
		}
		// 3. ��vao
		glBindVertexArray(geometry->getVao());

		// 4. ִ�л�������
		glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);

		//5. ���ǰ��vao��mprogram
		glBindVertexArray(0);
		shader->end();
	}
}


// ���Դ����Ⱦ
void Renderer::render(
	const std::vector<Mesh*>& meshes,
	Camera* camera,
	DirectionalLight* dirLight,
	const std::vector<PointLight*>& pointLights,
	SpotLight* spotLight,
	AmbientLight* ambLight
)
{
	//1. ���õ�ǰ֡���Ƶ�ʱ��opengl�ı�Ҫ״̬������
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//2. ������
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//3. ����mesh���л���
	for (int i = 0; i < meshes.size(); i++)
	{
		auto mesh = meshes[i];
		auto geometry = mesh->mGeometry;
		auto material = mesh->mMaterial;

		// 1. ����ʹ���ĸ�shader
		Shader* shader = pickShader(material->mType);

		// 2. ����shader��uniform
		shader->begin();

		switch (material->mType)
		{
		case MaterialType::PhongMaterial:
		{
			PhongMaterial* phongMaterial = (PhongMaterial*)material;

			// diffuse��ͼ��֡����
			GL_CALL(shader->setInt("sampler", 0));	// ������ȥ0������Ԫ������ɫ����
			// ������������Ԫ�ҹ�
			phongMaterial->mDiffuse->bind();

			// �߹��ɰ��֡����
			shader->setInt("specularMaskSampler", 1);
			phongMaterial->mSpecularMask->bind();

			// mvp�任����
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

			// ���߾���
			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			// ��Դ�����ĸ���

			//TODO: �۹�Ʋ�������
			shader->setVector3("spotLight.position", spotLight->getPosition());
			shader->setVector3("spotLight.color", spotLight->mColor);
			shader->setVector3("spotLight.targetDirection", spotLight->mTargetDirection);
			shader->setFloat("spotLight.innerLine", glm::cos(glm::radians(spotLight->mInnerAngle)));
			shader->setFloat("spotLight.outerLine", glm::cos(glm::radians(spotLight->mOuterAngle)));
			// ���ǿ��
			shader->setFloat("spotLight.specularIntensity", spotLight->mSpecularIntensity);

			//TODO: ƽ�й��������
			shader->setVector3("directionLight.color", dirLight->mColor);
			shader->setVector3("directionLight.direction", dirLight->mDirection);
			shader->setFloat("directionLight.specularIntensity", dirLight->mSpecularIntensity);

			//TODO: ���Դ��������
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

			// ��ߴ�С
			shader->setFloat("shiness", phongMaterial->mShiness);

			// ������
			GL_CALL(shader->setVector3("AmbientColor", ambLight->mColor));

			// ���λ�õĸ���
			GL_CALL(shader->setVector3("cameraPosition", camera->mPosition));
		}
		break;
		case MaterialType::WhiteMaterial:
		{
			// mvp�任����
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));
		}
		break;
		default:
			continue;

		}
		// 3. ��vao
		glBindVertexArray(geometry->getVao());

		// 4. ִ�л�������
		glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);

		//5. ���ǰ��vao��mprogram
		glBindVertexArray(0);
		shader->end();
	}
}


// ƽ�й����Ⱦ
void Renderer::render(
	Scene* scene,
	Camera* camera,
	DirectionalLight* dirLight,
	AmbientLight* ambLight,
	unsigned int fbo
)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	// ������ȼ��
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);	// �������д��
	// �ر����ƫ�ƹ���
	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_POLYGON_OFFSET_LINE);
	//ģ�����״̬
	glEnable(GL_STENCIL_TEST);		// ����ģ�����
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);	// ���û���д��״̬
	glStencilMask(0xFF);	//  ��ģ�����д�롣��֤��ģ�建����Ա�����
	// Ĭ����ɫ���״̬
	glDisable(GL_BLEND);
	

	//2. ������
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// �����������
	mOpacityObjects.clear();
	mTransparentObjects.clear();

	// ��������scene������͸�������͸������װ�뵽��ͬ������
	projectObject(scene);

	// ��͸��������������򣨸�������ľ���z����ֵ�����ҴӺ���ǰ����
	std::sort(mTransparentObjects.begin(),
		mTransparentObjects.end(),
		[camera](const Mesh* a,const Mesh* b) ->int
		{
			auto viewMatrix = camera->getViewMatrix();
			// ����a�����ϵ��z(���)(�����ʼ���Ƿ���(0,0,0)�㣬��ͨ���任������λ��)
			auto modelMatrixA = a->getModelMatrix();
			auto worldPositionA = modelMatrixA * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			auto cameraPositionA = viewMatrix * worldPositionA;

			// ����b�����ϵ��z(���)
			auto modelMatrixB = b->getModelMatrix();
			auto worldPositionB = modelMatrixB * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			auto cameraPositionB = viewMatrix * worldPositionB;

			return cameraPositionA.z < cameraPositionB.z;	// ����-z�ᣬԽС˵������Խ���Ȼ��ƣ�
		}
	);

	//3. ��Ⱦ��������
	// ��Ⱦʵ��(��͸��)�������
	for (int i = 0; i < mOpacityObjects.size(); i++)
	{
		renderObject(mOpacityObjects[i], camera, dirLight, ambLight);
	}
	// ��Ⱦ͸���������
	for (int i = 0; i < mTransparentObjects.size(); i++)
	{
		renderObject(mTransparentObjects[i], camera, dirLight, ambLight);
	}
	
}

// ��Ⱦһ������
void Renderer::renderObject(
	Object* object,
	Camera* camera,
	DirectionalLight* dirLight,
	AmbientLight* ambLight
)
{
	// 1. �ж���Mesh����Object,�����Mesh����Ҫ��Ⱦ
	if (object->getType() == ObjectType::Mesh 
		|| object->getType()==ObjectType::InstanceMesh
		|| object->getType()==ObjectType::InstanceUMesh)
	{
		auto mesh = reinterpret_cast<Mesh*>(object);
		auto geometry = mesh->mGeometry;

		Material* material = nullptr;
		// �����Ƿ���ȫ�ֲ���
		if (mGlobalMaterial != nullptr)
		{
			material = mGlobalMaterial;
		}
		else
		{
			material = mesh->mMaterial;
		}
		

		// ���õ�ǰ֡���Ƶ�ʱ��opengl�ı�Ҫ״̬������
		
		// (1) ������״̬
		setDepthState(material);
		// (2) ���ƫ��״̬PolygonOffset
		setPolygonOffsetState(material);
		// (3) ���ģ�����״̬ StencilTest
		setStencilState(material);
		// (4) ��ɫ���Blend
		setBlendState(material);
		// (5) ���޳�CullFace
		setCullinghFaceState(material);


		// 1. ����ʹ���ĸ�shader
		Shader* shader = pickShader(material->mType);

		// 2. ����shader��uniform
		shader->begin();

		switch (material->mType)
		{
		case MaterialType::PhongMaterial:
		{
			PhongMaterial* phongMaterial = (PhongMaterial*)material;

			// diffuse��ͼ��֡����
			GL_CALL(shader->setInt("sampler", 0));	// ������ȥ0������Ԫ������ɫ����
			// ������������Ԫ�ҹ�
			phongMaterial->mDiffuse->bind();

			//// �߹��ɰ��֡����
			//shader->setInt("specularMaskSampler", 1);
			//phongMaterial->mSpecularMask->bind();

			// mvp�任����
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

			// ���߾���
			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			// ��Դ�����ĸ���
			//TODO: ƽ�й��������
			shader->setVector3("directionLight.color", dirLight->mColor);
			shader->setVector3("directionLight.direction", dirLight->mDirection);
			shader->setFloat("directionLight.specularIntensity", dirLight->mSpecularIntensity);
			shader->setFloat("directionLight.intensity", dirLight->mInstensity); // �������ǿ�ȸ���
			
			// ��ߴ�С
			shader->setFloat("shiness", phongMaterial->mShiness);
			// ������
			GL_CALL(shader->setVector3("AmbientColor", ambLight->mColor));

			// ���λ�õĸ���
			GL_CALL(shader->setVector3("cameraPosition", camera->mPosition));
			// ��ɫ��ϲ�������
			shader->setFloat("Opacity", material->mOpacity);	
		}
			break;
		case MaterialType::WhiteMaterial:
		{
			// mvp�任����
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));
		}
			break;
		case MaterialType::DepthMaterial:
		{
			// mvp�任����
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

			// ���½�����ƽ���Զ����ƽ�����
			shader->setFloat("near", camera->mNear);
			shader->setFloat("far", camera->mFar);
		}
			break;
		case MaterialType::OpacityMaskMaterial:
		{
			OpacityMaskMaterial* OpacityMaterial = reinterpret_cast<OpacityMaskMaterial*>(material);

			// diffuse��ͼ��֡����
			GL_CALL(shader->setInt("sampler", 0));	// ������ȥ0������Ԫ������ɫ����
			// ������������Ԫ�ҹ�
			OpacityMaterial->mDiffuse->bind();

			// opacityMask��֡����
			shader->setInt("opacityMaskSampler", 1);
			OpacityMaterial->mOpacityMask->bind();

			// mvp�任����
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

			// ���߾���
			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			// ��Դ�����ĸ���
			//TODO: ƽ�й��������
			shader->setVector3("directionLight.color", dirLight->mColor);
			shader->setVector3("directionLight.direction", dirLight->mDirection);
			shader->setFloat("directionLight.specularIntensity", dirLight->mSpecularIntensity);

			// ��ߴ�С
			shader->setFloat("shiness", OpacityMaterial->mShiness);
			// ������
			GL_CALL(shader->setVector3("AmbientColor", ambLight->mColor));

			// ���λ�õĸ���
			GL_CALL(shader->setVector3("cameraPosition", camera->mPosition));
			// ��ɫ��ϲ�������
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
			// mvp�任����
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

			// diffuse��ͼ��֡����
			GL_CALL(shader->setInt("sampler", 0));	// ������ȥ0������Ԫ������ɫ����
			// ������������Ԫ�ҹ�
			phongEnvMaterial->mDiffuse->bind();
			
			shader->setInt("envSampler", 1);
			phongEnvMaterial->mEnv->bind();

			// mvp�任����
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

			// ���߾���
			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			// ��Դ�����ĸ���
			//TODO: ƽ�й��������
			shader->setVector3("directionLight.color", dirLight->mColor);
			shader->setVector3("directionLight.direction", dirLight->mDirection);
			shader->setFloat("directionLight.specularIntensity", dirLight->mSpecularIntensity);

			// ��ߴ�С
			shader->setFloat("shiness", phongEnvMaterial->mShiness);

			// ���λ�õĸ���
			GL_CALL(shader->setVector3("cameraPosition", camera->mPosition));
			// ��ɫ��ϲ�������
			shader->setFloat("Opacity", material->mOpacity);
		}
			break;
		case MaterialType::SpMapMaterial:
		{
			SpMapMaterial* spMat = reinterpret_cast<SpMapMaterial*>(material);
			mesh->setPosition(camera->mPosition);
			// mvp�任����
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
			
			// diffuse��ͼ��֡����
			GL_CALL(shader->setInt("sampler", 0));	// ������ȥ0������Ԫ������ɫ����
			// ������������Ԫ�ҹ�
			InstanceMaterial->mDiffuse->bind();

			//// �߹��ɰ��֡����
			//shader->setInt("specularMaskSampler", 1);
			//phongMaterial->mSpecularMask->bind();

			// mvp�任����
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

			// ���߾���
			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			// ��Դ�����ĸ���
			//TODO: ƽ�й��������
			shader->setVector3("directionLight.color", dirLight->mColor);
			shader->setVector3("directionLight.direction", dirLight->mDirection);
			shader->setFloat("directionLight.specularIntensity", dirLight->mSpecularIntensity);

			// ��ߴ�С
			shader->setFloat("shiness", InstanceMaterial->mShiness);
			// ������
			GL_CALL(shader->setVector3("AmbientColor", ambLight->mColor));

			// ���λ�õĸ���
			GL_CALL(shader->setVector3("cameraPosition", camera->mPosition));
			// ��ɫ��ϲ�������(͸����)
			shader->setFloat("Opacity", material->mOpacity);
		}
			break;
		case  MaterialType::PhongInstanceUMaterial:
		{
			PhongInstanceUMaterial* InstanceUMaterial = reinterpret_cast<PhongInstanceUMaterial*>(material);
			InstanceUMesh* UinstanceMesh = reinterpret_cast<InstanceUMesh*>(mesh);

			// diffuse��ͼ��֡����
			GL_CALL(shader->setInt("sampler", 0));	// ������ȥ0������Ԫ������ɫ����
			// ������������Ԫ�ҹ�
			InstanceUMaterial->mDiffuse->bind();

			//// �߹��ɰ��֡����
			//shader->setInt("specularMaskSampler", 1);
			//phongMaterial->mSpecularMask->bind();

			// mvp�任����
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

			// ���߾���
			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			// ��Դ�����ĸ���
			//TODO: ƽ�й��������
			shader->setVector3("directionLight.color", dirLight->mColor);
			shader->setVector3("directionLight.direction", dirLight->mDirection);
			shader->setFloat("directionLight.specularIntensity", dirLight->mSpecularIntensity);

			// ��ߴ�С
			shader->setFloat("shiness", InstanceUMaterial->mShiness);
			// ������
			GL_CALL(shader->setVector3("AmbientColor", ambLight->mColor));

			// ���λ�õĸ���
			GL_CALL(shader->setVector3("cameraPosition", camera->mPosition));
			// ��ɫ��ϲ�������(͸����)
			shader->setFloat("Opacity", material->mOpacity);

			//����uniform���͵ľ���任����
			shader->setMatrix4x4Array("matrices", UinstanceMesh->mInstanceMatrices,UinstanceMesh->mInstanceCount);
		}
			break;
		case MaterialType::GrassInstanceMaterial:
		{
			GrassInstanceMaterial* grassMat = reinterpret_cast<GrassInstanceMaterial*>(material);
			InstanceMesh* instanceMesh = reinterpret_cast<InstanceMesh*>(mesh);
			instanceMesh->sortMatrices(camera->getViewMatrix());
			instanceMesh->updataMatrices();

			// diffuse��ͼ��֡����
			GL_CALL(shader->setInt("sampler", 0));	// ������ȥ0������Ԫ������ɫ����
			// ������������Ԫ�ҹ�
			grassMat->mDiffuse->bind();
			
			// ͸���ɰ�
			shader->setInt("opacityMask", 1);
			grassMat->mOpacityMask->bind();

			

			// mvp�任����
			GL_CALL(shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix()));
			GL_CALL(shader->setMatrix4x4("viewMatrix", camera->getViewMatrix()));
			GL_CALL(shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix()));

			// �ݵ���ͼUV����
			shader->setFloat("uvScale", grassMat->mUVscale);
			shader->setFloat("brightness", grassMat->mBrightness);
			shader->setFloat("time", glfwGetTime());
			//���������ǿ��
			shader->setVector3("windDirection", grassMat->mWindDirection);
			shader->setFloat("windScale", grassMat->mWindScale);
			shader->setFloat("phaseScale", grassMat->mPhaseScale);	// �ݵ���λ��С

			// ��Դ�����ĸ���
			//TODO: ƽ�й��������
			shader->setVector3("directionLight.color", dirLight->mColor);
			shader->setVector3("directionLight.direction", dirLight->mDirection);
			shader->setFloat("directionLight.specularIntensity", dirLight->mSpecularIntensity);

			// ��ߴ�С
			shader->setFloat("shiness", grassMat->mShiness);
			// ������
			GL_CALL(shader->setVector3("AmbientColor", ambLight->mColor));

			// ���λ�õĸ���
			GL_CALL(shader->setVector3("cameraPosition", camera->mPosition));
			// ��ɫ��ϲ�������(͸����)
			shader->setFloat("Opacity", material->mOpacity);

			// �Ʋ��ɰ�
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

		// 3. ��vao
		glBindVertexArray(geometry->getVao());

		// 4. ִ�л�������
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

		//5. ���ǰ��vao��mprogram
		glBindVertexArray(0);
		shader->end();
	}

	//// 2. ����object���ӽڵ㣬��ÿ���ӽڵ㶼��Ҫ����renderObject
	//std::vector<Object*> children = object->getChildren();
	//for (int i = 0; i < children.size(); i++)
	//{
	//	renderObject(children[i], camera, dirLight, ambLight);
	//}
}

// ������Ȳ���״̬
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

// �������ƫ��״̬
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

// ������ɫ���״̬
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

// �������޳�
void Renderer::setCullinghFaceState(Material* material)
{
	if (material->mFaceCulling)
	{
		glEnable(GL_CULL_FACE);		// �������޳�
		glFrontFace(material->mFrontFace);
		glCullFace(material->mCullFace);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
}

