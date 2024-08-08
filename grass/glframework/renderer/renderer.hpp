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

// 渲染器类
class Renderer
{
public:
	Renderer();
	~Renderer();

public:
	// 渲染函数：每次调用都会渲染一帧
	// 平行光的渲染
	void render(
		const std::vector<Mesh*>& meshes,
		Camera* camera,
		DirectionalLight* dirLight,
		AmbientLight* ambLight
	);

	// 点光源的渲染
	void Renderer::render(
		const std::vector<Mesh*>& meshes,
		Camera* camera,
		PointLight* pointLight,
		AmbientLight* ambLight
	);

	// 聚光灯的渲染
	void Renderer::render(
		const std::vector<Mesh*>& meshes,
		Camera* camera,
		DirectionalLight* dirLight,
		PointLight* pointLight,
		SpotLight* spotLight,
		AmbientLight* ambLight
	);

	// 多光源的渲染
	void Renderer::render(
		const std::vector<Mesh*>& meshes,
		Camera* camera,
		DirectionalLight* dirLight,
		const std::vector<PointLight*>& pointLights,
		SpotLight* spotLight,
		AmbientLight* ambLight
	);

	// 平行光的渲染
	void render(
		Scene* scene,
		Camera* camera,
		DirectionalLight* dirLight,
		AmbientLight* ambLight,
		unsigned int fbo=0
	);
	// 渲染一个物体
	void renderObject(
		Object* object,
		Camera* camera,
		DirectionalLight* dirLight,
		AmbientLight* ambLight
	);
	// 设置画布清理颜色
	void setClearColor(glm::vec3 color);

public:
	Material* mGlobalMaterial{ nullptr };	// 全局材质,所有Mesh都使用这个全局材质

private:
	// 解析scene，将不透明物体和透明物体装入不同队列中
	void projectObject(Object* obj);
	// 根据material类型不同，选择不同的shader
	Shader* pickShader(MaterialType type);
	// 设置深度测试状态
	void setDepthState(Material* material);
	// 设置深度偏移状态
	void setPolygonOffsetState(Material* material);
	// 设置模板测试状态
	void setStencilState(Material* material);
	// 设置颜色混合状态
	void setBlendState(Material* material);
	// 设置面剔除
	void setCullinghFaceState(Material* material);

private:
	// 生成多种不同的shader对象
	// 根据材质类型的不同，挑选使用哪一个shader对象
	Shader* mPhongShader{ nullptr };		// 冯氏光照
	Shader* mWhiteShader{ nullptr };		// 点光源（纯白色）
	Shader* mDepthShader{ nullptr };		// 深度测试
	Shader* mOpacityMaskShader{ nullptr };	// 透明度蒙版
	Shader* mScreenShader{ nullptr };		// 帧缓冲
	Shader* mCubeShader{ nullptr };			// 立方体贴图
	Shader* mPhongEnvShader{ nullptr };		// 立方体贴图进行采样作为环境光
	Shader* mSpmapShader{ nullptr };		// 球形贴图做天空盒
	Shader* mInstanceShader{ nullptr };		// 绘制实例的shader(使用attribute方式)
	Shader* mUInstanceShader{ nullptr };	// 绘制实例的shader(使用Uniform方式)
	Shader* mGrassInstanceShader{ nullptr };		// 绘制草丛

	// 不透明物体与透明物体的队列（每一帧会之前，需要清空两个队列）
	std::vector<Mesh*> mOpacityObjects{};		// 不透明物体
	std::vector<Mesh*> mTransparentObjects{};	// 透明物体
};
