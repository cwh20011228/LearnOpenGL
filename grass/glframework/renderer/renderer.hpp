#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "../core.h"
#include "../../wrapper/checkError.h"
#include "../mesh/mesh.hpp"
#include "../mesh/instanceMesh.hpp"
#include "../mesh/instanceUMesh.hpp"
#include "../../application/camera/camera.hpp"
#include "../light/light.hpp"
#include "../light/directionalLight.hpp"
#include "../light/pointLight.hpp"
#include "../light/spotLight.hpp"
#include "../light/ambientLight.hpp"
#include "../shader.h"
#include "../material/phongMaterial.hpp"
#include "../material/whiteMaterial.hpp"
#include "../material/OpacityMaskMaterial.hpp"
#include "../material/screenMaterial.hpp"
#include "../material/cubeMaterial.hpp"
#include "../material/PhongEnvMaterial.hpp"
#include "../material/SpMapMaterial.hpp"
#include "../material/PhongInstanceMaterial.hpp"
#include "../material/PhongInstanceUMaterial.hpp"
#include "../material/grassInstanceMaterial.hpp"
#include "../scene.hpp"

// ��Ⱦ����
class Renderer
{
public:
	Renderer();
	~Renderer();

public:
	// ��Ⱦ������ÿ�ε��ö�����Ⱦһ֡
	// ƽ�й����Ⱦ
	void render(
		const std::vector<Mesh*>& meshes,
		Camera* camera,
		DirectionalLight* dirLight,
		AmbientLight* ambLight
	);

	// ���Դ����Ⱦ
	void Renderer::render(
		const std::vector<Mesh*>& meshes,
		Camera* camera,
		PointLight* pointLight,
		AmbientLight* ambLight
	);

	// �۹�Ƶ���Ⱦ
	void Renderer::render(
		const std::vector<Mesh*>& meshes,
		Camera* camera,
		DirectionalLight* dirLight,
		PointLight* pointLight,
		SpotLight* spotLight,
		AmbientLight* ambLight
	);

	// ���Դ����Ⱦ
	void Renderer::render(
		const std::vector<Mesh*>& meshes,
		Camera* camera,
		DirectionalLight* dirLight,
		const std::vector<PointLight*>& pointLights,
		SpotLight* spotLight,
		AmbientLight* ambLight
	);

	// ƽ�й����Ⱦ
	void render(
		Scene* scene,
		Camera* camera,
		DirectionalLight* dirLight,
		AmbientLight* ambLight,
		unsigned int fbo=0
	);
	// ��Ⱦһ������
	void renderObject(
		Object* object,
		Camera* camera,
		DirectionalLight* dirLight,
		AmbientLight* ambLight
	);
	// ���û���������ɫ
	void setClearColor(glm::vec3 color);

public:
	Material* mGlobalMaterial{ nullptr };	// ȫ�ֲ���,����Mesh��ʹ�����ȫ�ֲ���

private:
	// ����scene������͸�������͸������װ�벻ͬ������
	void projectObject(Object* obj);
	// ����material���Ͳ�ͬ��ѡ��ͬ��shader
	Shader* pickShader(MaterialType type);
	// ������Ȳ���״̬
	void setDepthState(Material* material);
	// �������ƫ��״̬
	void setPolygonOffsetState(Material* material);
	// ����ģ�����״̬
	void setStencilState(Material* material);
	// ������ɫ���״̬
	void setBlendState(Material* material);
	// �������޳�
	void setCullinghFaceState(Material* material);

private:
	// ���ɶ��ֲ�ͬ��shader����
	// ���ݲ������͵Ĳ�ͬ����ѡʹ����һ��shader����
	Shader* mPhongShader{ nullptr };		// ���Ϲ���
	Shader* mWhiteShader{ nullptr };		// ���Դ������ɫ��
	Shader* mDepthShader{ nullptr };		// ��Ȳ���
	Shader* mOpacityMaskShader{ nullptr };	// ͸�����ɰ�
	Shader* mScreenShader{ nullptr };		// ֡����
	Shader* mCubeShader{ nullptr };			// ��������ͼ
	Shader* mPhongEnvShader{ nullptr };		// ��������ͼ���в�����Ϊ������
	Shader* mSpmapShader{ nullptr };		// ������ͼ����պ�
	Shader* mInstanceShader{ nullptr };		// ����ʵ����shader(ʹ��attribute��ʽ)
	Shader* mUInstanceShader{ nullptr };	// ����ʵ����shader(ʹ��Uniform��ʽ)
	Shader* mGrassInstanceShader{ nullptr };		// ���Ʋݴ�

	// ��͸��������͸������Ķ��У�ÿһ֡��֮ǰ����Ҫ����������У�
	std::vector<Mesh*> mOpacityObjects{};		// ��͸������
	std::vector<Mesh*> mTransparentObjects{};	// ͸������
};
