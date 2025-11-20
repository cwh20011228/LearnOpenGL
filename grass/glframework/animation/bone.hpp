#pragma once

#include "../core.h"

struct KeyPostion
{
	glm::vec3	m_position;
	float		m_timeStamp;
};

struct KeyRotation
{
	glm::quat	m_orientation;
	float		m_timeStamp;
};

struct KeyScale
{
	glm::vec3	m_scale;
	float		m_timeStamp;
};

struct BoneInfo
{
	unsigned int	m_id;
	glm::mat4	m_offsetMatrix;
	BoneInfo() :m_id(0), m_offsetMatrix(1.0f) {}
};

class Bone
{
public:
	Bone(std::string _name, unsigned int _id, aiNodeAnim* _boneAnim);
	~Bone();

	// 每一个bone所对应的动画叫做channel;一组动画叫做Animation
	void InitAnimation(const aiNodeAnim* _channel);
	void updata(float _time);

private:
	// 获取插值比例
	float getLerpFactor(float _lastTime, float _nextTime, float _curTime);

	// 计算本地变换
	glm::mat4 interpolatePosition(float _time);
	glm::mat4 interpolateRotation(float _time);
	glm::mat4 interpolateScale(float _time);

	// 获取当前时间的last和next的关键帧
	unsigned int getPositionIndexByTime(float _time);
	unsigned int getRotationIndexByTime(float _time);
	unsigned int getScaleIndexByTime(float _time);

private:
	std::vector<KeyPostion>		m_positionArr;
	std::vector<KeyRotation>	m_rotationArr;
	std::vector<KeyScale>		m_scaleArr;

	glm::mat4					m_localTransform;
	std::string					m_name;
	unsigned int						m_id;
};

