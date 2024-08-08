#include "./object.hpp"


Object::Object()
{
	mType = ObjectType::Object;
}

Object::~Object()
{

}

void Object::setPosition(glm::vec3 pos)
{
	mPosition = pos;
}

glm::vec3 Object::getPosition() const
{
	return mPosition;
}

// 增量旋转
void Object::rotateX(float angle)
{
	mAngleX += angle;
}
void Object::rotateY(float angle)
{
	mAngleY += angle;
}
void Object::rotateZ(float angle)
{
	mAngleZ += angle;
}

void Object::setScale(glm::vec3 scale)	// 设置缩放程度（x,y,z轴上的缩放）
{
	mScale = scale;
}


// 设置旋转角
void Object::setAngleX(float angle)
{
	mAngleX = angle;
}
void Object::setAngleY(float angle)
{
	mAngleY = angle;
}
void Object::setAngleZ(float angle)
{
	mAngleZ = angle;
}


// unity：缩放	旋转 （缩放，旋转后，它的局部坐标系下的x,y,z轴会变） 平移（要在世界坐标系下平移）
glm::mat4 Object::getModelMatrix() const	// 得到模型变换矩阵
{
	// 获取父亲的变换矩阵
	glm::mat4 parentModelMatrix{ 1.0f };
	if (mParent != nullptr)
	{
		parentModelMatrix = mParent->getModelMatrix();
	}
	glm::mat4 transform{ 1.0f };

	// 缩放
	transform = glm::scale(transform, mScale);

	// 旋转 pitch  yaw  roll
	transform = glm::rotate(transform, glm::radians(mAngleX), glm::vec3(1.0f, 0.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(mAngleY), glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(mAngleZ), glm::vec3(0.0f, 0.0f, 1.0f));

	// 在世界坐标系中平移
	transform = parentModelMatrix*glm::translate(glm::mat4(1.0f), mPosition) * transform;

	return transform;
}

// 插入孩子节点
void Object::addChild(Object* obj)
{
	// 1.检查是否曾经加入过这个孩子
	std::vector<Object*>::iterator iter = std::find(mChildren.begin(), mChildren.end(), obj);
	if (iter != mChildren.end())
	{
		std::cerr << "Duplicated Child added." << std::endl;
		return;
	}
	// 2.加入孩子
	mChildren.push_back(obj);
	// 3. 告诉新加入孩子，它的爸爸是谁
	obj->mParent = this;
}

// 获取孩子节点数组
std::vector<Object*> Object::getChildren()
{
	return mChildren;
}

// 获取父亲节点
Object* Object::getParent()
{
	return mParent;
}