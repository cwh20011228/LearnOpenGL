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

// ������ת
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

void Object::setScale(glm::vec3 scale)	// �������ų̶ȣ�x,y,z���ϵ����ţ�
{
	mScale = scale;
}


// ������ת��
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


// unity������	��ת �����ţ���ת�����ľֲ�����ϵ�µ�x,y,z���䣩 ƽ�ƣ�Ҫ����������ϵ��ƽ�ƣ�
glm::mat4 Object::getModelMatrix() const	// �õ�ģ�ͱ任����
{
	// ��ȡ���׵ı任����
	glm::mat4 parentModelMatrix{ 1.0f };
	if (mParent != nullptr)
	{
		parentModelMatrix = mParent->getModelMatrix();
	}
	glm::mat4 transform{ 1.0f };

	// ����
	transform = glm::scale(transform, mScale);

	// ��ת pitch  yaw  roll
	transform = glm::rotate(transform, glm::radians(mAngleX), glm::vec3(1.0f, 0.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(mAngleY), glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(mAngleZ), glm::vec3(0.0f, 0.0f, 1.0f));

	// ����������ϵ��ƽ��
	transform = parentModelMatrix*glm::translate(glm::mat4(1.0f), mPosition) * transform;

	return transform;
}

// ���뺢�ӽڵ�
void Object::addChild(Object* obj)
{
	// 1.����Ƿ�����������������
	std::vector<Object*>::iterator iter = std::find(mChildren.begin(), mChildren.end(), obj);
	if (iter != mChildren.end())
	{
		std::cerr << "Duplicated Child added." << std::endl;
		return;
	}
	// 2.���뺢��
	mChildren.push_back(obj);
	// 3. �����¼��뺢�ӣ����İְ���˭
	obj->mParent = this;
}

// ��ȡ���ӽڵ�����
std::vector<Object*> Object::getChildren()
{
	return mChildren;
}

// ��ȡ���׽ڵ�
Object* Object::getParent()
{
	return mParent;
}