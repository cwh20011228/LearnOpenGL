#include "bone.hpp"

Bone::Bone(std::string _name, unsigned int _id, aiNodeAnim* _boneAnim)
	:m_name(_name),
	m_id(_id),
	m_localTransform(1.0)
{
	this->InitAnimation(_boneAnim);
}
