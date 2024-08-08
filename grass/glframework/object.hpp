#pragma once

#include "core.h"

enum class ObjectType
{
	Object,
	Mesh,
	Scene,
	InstanceMesh,	// 使用attribute
	InstanceUMesh	// 使用uniform方式
};

class Object
{
public:
	Object();
	~Object();
public:
	void setPosition(glm::vec3 pos);
	glm::vec3 getPosition()const;
	// 增量旋转
	void rotateX(float angle);
	void rotateY(float angle);
	void rotateZ(float angle);

	// 设置旋转角
	void setAngleX(float angle);
	void setAngleY(float angle);
	void setAngleZ(float angle);

	// 设置缩放程度（x,y,z轴上的缩放）
	void setScale(glm::vec3 scale);		
	// 得到模型变换矩阵
	glm::mat4 getModelMatrix() const;		
	// 获取类型信息
	ObjectType getType() const { return mType; }
	// 插入孩子节点
	void addChild(Object* obj);	
	// 获取孩子节点数组
	std::vector<Object*> getChildren();
	// 获取父亲节点
	Object* getParent();
	
protected:
	glm::vec3 mPosition{ 0.0f };	// 世界坐标系下的位置坐标
	
	// unity旋转标准：pitch（绕着x轴上下旋转）	yaw（绕着y轴左右旋转）	roll（绕着z轴旋转）
	float mAngleX{ 0.0f };
	float mAngleY{ 0.0f };
	float mAngleZ{ 0.0f };

	glm::vec3 mScale{ 1.0f };	// 缩放

	std::vector<Object*> mChildren{};	// 子节点数组
	Object* mParent{ nullptr };			// 父亲节点
	ObjectType mType;	// 类型记录
};