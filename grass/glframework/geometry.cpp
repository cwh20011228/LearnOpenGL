#include "geometry.hpp"
#include <vector>

Geometry::Geometry()
{

}

Geometry::Geometry(
    const std::vector<float>& positions,
    const std::vector<float>& normals,
    const std::vector<float>& uvs,
    const std::vector<unsigned int>& indices
)
{ 
    mIndicesCount = indices.size();
    // 创建VBO
    glGenBuffers(1, &mPosVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mPosVbo);
    glBufferData(GL_ARRAY_BUFFER, positions.size()*sizeof(float), positions.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &mUvVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mUvVbo);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), uvs.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &mNormalVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mNormalVbo);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);

    // 创建EBO
    glGenBuffers(1, &mEbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // 创建VAo
    glGenVertexArrays(1, &mVao);
    glBindVertexArray(mVao);

    // 加入位置属性描述信息
    glBindBuffer(GL_ARRAY_BUFFER, mPosVbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // 加入uv属性描述信息
    glBindBuffer(GL_ARRAY_BUFFER, mUvVbo);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    // 加入法线属性描述信息
    glBindBuffer(GL_ARRAY_BUFFER, mNormalVbo);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // 加入ebo到当前vao中
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);

    // 解绑当前的vao
    glBindVertexArray(0);
}

Geometry::Geometry(
    const std::vector<float>& positions,
    const std::vector<float>& normals,
    const std::vector<float>& uvs,
    const std::vector<float>& colors,
    const std::vector<unsigned int>& indices
)
{
    mIndicesCount = indices.size();
    // 创建VBO
    glGenBuffers(1, &mPosVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mPosVbo);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &mUvVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mUvVbo);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), uvs.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &mNormalVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mNormalVbo);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &mColorVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mColorVbo);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);

    // 创建EBO
    glGenBuffers(1, &mEbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // 创建VAo
    glGenVertexArrays(1, &mVao);
    glBindVertexArray(mVao);

    // 加入位置属性描述信息
    glBindBuffer(GL_ARRAY_BUFFER, mPosVbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // 加入uv属性描述信息
    glBindBuffer(GL_ARRAY_BUFFER, mUvVbo);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    // 加入法线属性描述信息
    glBindBuffer(GL_ARRAY_BUFFER, mNormalVbo);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // 加入颜色属性描述信息
    glBindBuffer(GL_ARRAY_BUFFER, mColorVbo);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // 加入ebo到当前vao中
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);

    // 解绑当前的vao
    glBindVertexArray(0);
}


Geometry::~Geometry()
{
	if (mVao != 0)
	{
		glDeleteVertexArrays(1, &mVao);
	}

	if (mPosVbo != 0)
	{
		glDeleteBuffers(1, &mPosVbo);
	}
	
	if (mUvVbo != 0)
	{
		glDeleteBuffers(1, &mUvVbo);
	}

    if (mColorVbo != 0)
    {
        glDeleteBuffers(1, &mColorVbo);
    }
	
	if (mEbo != 0)
	{
		glDeleteBuffers(1, &mEbo);
	}
    if (mNormalVbo != 0)
    {
        glDeleteBuffers(1, &mNormalVbo);
    }
}

Geometry* Geometry::createBox(float size)
{
	Geometry* geometry = new Geometry();
    geometry->mIndicesCount = 36;
	float halfSize = size / 2.0;

    float vertices[] = {
        // 前面
        -halfSize, -halfSize, halfSize, 
        halfSize, -halfSize, halfSize, 
        halfSize, halfSize, halfSize, 
        -halfSize, halfSize, halfSize,

        // 后面
        -halfSize, -halfSize, -halfSize,
        -halfSize, halfSize, -halfSize, 
        halfSize, halfSize, -halfSize,
        halfSize, -halfSize, -halfSize,

        // 上面
        -halfSize, halfSize, halfSize, 
        halfSize, halfSize, halfSize,
        halfSize, halfSize, -halfSize,
        -halfSize, halfSize, -halfSize,

        // 下面
        -halfSize, -halfSize, -halfSize, 
        halfSize, -halfSize, -halfSize,
        halfSize, -halfSize, halfSize, 
        -halfSize, -halfSize, halfSize,

        // 右面
        halfSize, -halfSize, halfSize,
        halfSize, -halfSize, -halfSize,
        halfSize, halfSize, -halfSize, 
        halfSize, halfSize, halfSize,

        // 左面
        -halfSize, -halfSize, -halfSize,
        -halfSize, -halfSize, halfSize,
        -halfSize, halfSize, halfSize, 
        -halfSize, halfSize, -halfSize
    };

    float uvs[] = {
        // 前面
        0.0f, 0.0f,  // 左下
        1.0f, 0.0f,  // 右下
        1.0f, 1.0f,  // 右上
        0.0f, 1.0f,  // 左上

        // 后面
        0.0f, 0.0f,  // 左下
        1.0f, 0.0f,  // 右下
        1.0f, 1.0f,  // 右上
        0.0f, 1.0f,  // 左上

        // 左侧
        0.0f, 0.0f,  // 左下
        1.0f, 0.0f,  // 右下
        1.0f, 1.0f,  // 右上
        0.0f, 1.0f,  // 左上

        // 右侧
        0.0f, 0.0f,  // 左下
        1.0f, 0.0f,  // 右下
        1.0f, 1.0f,  // 右上
        0.0f, 1.0f,  // 左上

        // 上面
        0.0f, 0.0f,  // 左下
        1.0f, 0.0f,  // 右下
        1.0f, 1.0f,  // 右上
        0.0f, 1.0f,  // 左上

        // 下面
        0.0f, 0.0f,  // 左下
        1.0f, 0.0f,  // 右下
        1.0f, 1.0f,  // 右上
        0.0f, 1.0f,  // 左上
    };

    float normals[] ={
        // 前面
        0.0f, 0.0f, 1.0f, 
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,

        // 后面
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,

        // 上面
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,

        // 下面
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,

        // 右侧
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,

        // 左侧
        -1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
    };
 
    unsigned int indices[] = {
        // 前面
        0, 1, 2,   2, 3, 0,
        // 后面
        4, 5, 6,   6, 7, 4,
        // 左侧
        8, 9, 10,  10, 11, 8,
        // 右侧
        12, 13, 14, 14, 15, 12,
        // 上面
        16, 17, 18, 18, 19, 16,
        // 下面
        20, 21, 22, 22, 23, 20
    };
    
    // 创建VBO

    glGenBuffers(1, &geometry->mPosVbo);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->mPosVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1,&geometry->mUvVbo);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->mUvVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);

    glGenBuffers(1, &geometry->mNormalVbo);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->mNormalVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

    // 创建EBO
    glGenBuffers(1, &geometry->mEbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 创建VAo
    glGenVertexArrays(1, &geometry->mVao);
    glBindVertexArray(geometry->mVao);

    // 加入位置属性描述信息
    glBindBuffer(GL_ARRAY_BUFFER, geometry->mPosVbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // 加入uv属性描述信息
    glBindBuffer(GL_ARRAY_BUFFER, geometry->mUvVbo);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    // 加入法线属性描述信息
    glBindBuffer(GL_ARRAY_BUFFER, geometry->mNormalVbo);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // 加入ebo到当前vao中
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->mEbo);

    glBindVertexArray(0);

	return geometry;
}

Geometry* Geometry::createSphere(float radius)
{
	Geometry* geometry = new Geometry();

    std::vector<GLfloat> positions{};
    std::vector<GLfloat> uvs{};
    std::vector<GLfloat> normals{};
    std::vector<GLuint> indices{};

    // 经线与纬线的数量
    int latitude = 60;  // 纬线
    int longitude = 60; // 经线

    // 计算位置和UV
    for (int i = 0; i <= latitude; i++)
    {
        for (int j = 0; j <= longitude; j++)
        {
            float phi = i * glm::pi<float>() / latitude;
            float theta = j* 2 * glm::pi<float>() / longitude;

            float x = radius * sin(phi) * cos(theta);
            float z = radius * sin(phi) * sin(theta);
            float y = radius * cos(phi);

            positions.push_back(x);
            positions.push_back(y);
            positions.push_back(z);

            float u = 1.0 - (float)j / (float)longitude;
            float v = 1.0 - (float)i / (float)latitude;

            uvs.push_back(u);
            uvs.push_back(v);

            // 法线方向没有问题，法线的长度不为1
            normals.push_back(x);
            normals.push_back(y);
            normals.push_back(z);

        }
    }

    // 计算索引
    for (int i = 0; i < latitude; i++)
    {
        for (int j = 0; j < longitude; j++)
        {
            int p1 = i * (longitude + 1) + j;
            int p2 = p1 + longitude + 1;
            int p3 = p1 + 1;
            int p4 = p2 + 1;

            indices.push_back(p1);
            indices.push_back(p2);
            indices.push_back(p3);

            indices.push_back(p3);
            indices.push_back(p2);
            indices.push_back(p4);
        }
    }

    geometry->mIndicesCount = indices.size();

    // 生成vbo,vao
    glGenBuffers(1, &geometry->mPosVbo);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->mPosVbo);
    glBufferData(GL_ARRAY_BUFFER, positions.size()*sizeof(float), positions.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &geometry->mUvVbo);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->mUvVbo);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), uvs.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &geometry->mNormalVbo);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->mNormalVbo);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &geometry->mEbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &geometry->mVao);
    glBindVertexArray(geometry->mVao);

    glBindBuffer(GL_ARRAY_BUFFER, geometry->mPosVbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, geometry->mUvVbo);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, geometry->mNormalVbo);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->mEbo);

    glBindVertexArray(0);

	return geometry;
}

Geometry* Geometry::createPlane(float width, float height)
{
    Geometry* geometry = new Geometry();
    geometry->mIndicesCount = 6;

    float halfW = width / 2.0f;
    float halfH = height / 2.0f;

    
    float positions[] =
    {
        -halfW,-halfH,0.0f,
        halfW,-halfH,0.0f,
        halfW,halfH,0.0f,
        -halfW,halfH,0.0f
    };

    float uvs[] =
    {
        0.0f,0.0f,
        1.0f,0.0f,
        1.0f,1.0f,
        0.0f,1.0f
    };

    GLfloat normals[] ={
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f
    };

    unsigned int indices[] =
    {
        0,1,2,
        2,3,0
    };

    // 生成vbo,vao
    glGenBuffers(1, &geometry->mPosVbo);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->mPosVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

    glGenBuffers(1, &geometry->mUvVbo);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->mUvVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);

    glGenBuffers(1, &geometry->mNormalVbo);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->mNormalVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals,GL_STATIC_DRAW);

    glGenBuffers(1, &geometry->mEbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 创建vao
    glGenVertexArrays(1, &geometry->mVao);
    glBindVertexArray(geometry->mVao);

    // 将vbo描述加入到vao中
    glBindBuffer(GL_ARRAY_BUFFER, geometry->mPosVbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, geometry->mUvVbo);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, geometry->mNormalVbo);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

    // 将Ebo加入到当前的vao中
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->mEbo);

    glBindVertexArray(0);   // 将当前的vao解绑

    return geometry;
}


// 铺满屏幕的平面
Geometry* Geometry::createScreenPlane()
{
    Geometry* geometry = new Geometry();
    geometry->mIndicesCount = 6;

    float positions[] =
    {
        -1.0f, 1.0f,
        -1.0f, -1.0f, 
        1.0f, -1.0f, 
        1.0f, 1.0f
    };

    float uvs[] =
    {
        0.0f,1.0f,
        0.0f,0.0f,
        1.0f,0.0f,
        1.0f,1.0f
    };

    unsigned int indices[] =
    {
        0,1,2,
        0,2,3
    };

    GLuint& posVbo = geometry->mPosVbo;
    GLuint& uvVbo = geometry->mUvVbo;
    GLuint& ebo = geometry->mEbo;
    GLuint& vao = geometry->mVao;

    glGenBuffers(1, &posVbo);
    glBindBuffer(GL_ARRAY_BUFFER, posVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

    glGenBuffers(1, &uvVbo);
    glBindBuffer(GL_ARRAY_BUFFER, uvVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, posVbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);


    glBindBuffer(GL_ARRAY_BUFFER, uvVbo);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBindVertexArray(0);

    return geometry;
}