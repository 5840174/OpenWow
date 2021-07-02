#pragma once

#include "M2_Types.h"
#include "M2_Part_Bone.h"

// FORWARD BEGIN
class CM2;
// FORWARD END

class SM2_Part_Event_Wrapper
{
public:
	SM2_Part_Event_Wrapper(const CM2& M2Object, const std::shared_ptr<IFile>& File, const SM2_Event& M2Event);
	virtual ~SM2_Part_Event_Wrapper();

private:
	char					m_ID[4];
	uint32					m_Data;
	std::weak_ptr<const CM2_Part_Bone>	m_Bone;
	glm::vec3				m_Position;
	M2_Animated<bool>		m_IsEnabled;

private:
	const CM2& m_M2Object;
	const SM2_Event& m_M2Event;
};
