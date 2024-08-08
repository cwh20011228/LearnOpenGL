#pragma once

#include "core.h"

enum class ObjectType
{
	Object,
	Mesh,
	Scene,
	InstanceMesh,	// ʹ��attribute
	InstanceUMesh	// ʹ��uniform��ʽ
};

class Object
{
public:
	Object();
	~Object();
public:
	void setPosition(glm::vec3 pos);
	glm::vec3 getPosition()const;
	// ������ת
	void rotateX(float angle);
	void rotateY(float angle);
	void rotateZ(float angle);

	// ������ת��
	void setAngleX(float angle);
	void setAngleY(float angle);
	void setAngleZ(float angle);

	// �������ų̶ȣ�x,y,z���ϵ����ţ�
	void setScale(glm::vec3 scale);		
	// �õ�ģ�ͱ任����
	glm::mat4 getModelMatrix() const;		
	// ��ȡ������Ϣ
	ObjectType getType() const { return mType; }
	// ���뺢�ӽڵ�
	void addChild(Object* obj);	
	// ��ȡ���ӽڵ�����
	std::vector<Object*> getChildren();
	// ��ȡ���׽ڵ�
	Object* getParent();
	
protected:
	glm::vec3 mPosition{ 0.0f };	// ��������ϵ�µ�λ������
	
	// unity��ת��׼��pitch������x��������ת��	yaw������y��������ת��	roll������z����ת��
	float mAngleX{ 0.0f };
	float mAngleY{ 0.0f };
	float mAngleZ{ 0.0f };

	glm::vec3 mScale{ 1.0f };	// ����

	std::vector<Object*> mChildren{};	// �ӽڵ�����
	Object* mParent{ nullptr };			// ���׽ڵ�
	ObjectType mType;	// ���ͼ�¼
};