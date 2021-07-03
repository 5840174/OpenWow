#pragma once

#include "M2_Types.h"
#include "M2_Part_Bone.h"

// FORWARD BEGIN
class CM2;
// FORWARD END

class CM2_Part_Event
{
public:
	CM2_Part_Event(const CM2& M2Object, const std::shared_ptr<IByteBuffer>& ByteBuffer, const SM2_Event& M2Event);
	virtual ~CM2_Part_Event();

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
