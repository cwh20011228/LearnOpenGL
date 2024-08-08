#pragma once

#include "core.h"

// 几何外观
class Geometry
{
public:
	Geometry();
	Geometry(
		const std::vector<float>& positions,
		const std::vector<float>& normals,
		const std::vector<float>& uvs,
		const std::vector<unsigned int>& indices
	);

	Geometry(
		const std::vector<float>& positions,
		const std::vector<float>& normals,
		const std::vector<float>& uvs,
		const std::vector<float>& colors,
		const std::vector<unsigned int>& indices
	);
	~Geometry();

	static Geometry* createBox(float size);
	static Geometry* createSphere(float radius);
	static Geometry* createPlane(float width, float height);
	// 创建铺满屏幕的平面
	static Geometry* createScreenPlane();	

	GLuint getVao() const { return mVao; }
	uint32_t getIndicesCount() const { return mIndicesCount; }
private:
	GLuint mVao{ 0 };
	GLuint mPosVbo{ 0 };
	GLuint mUvVbo{ 0 };
	GLuint mColorVbo{ 0 };
	GLuint mEbo{ 0 };
	GLuint mNormalVbo{ 0 };

	uint32_t mIndicesCount{ 0 };
};