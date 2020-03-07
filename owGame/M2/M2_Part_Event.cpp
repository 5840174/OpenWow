#include "stdafx.h"

// Include
#include "M2.h"

// General
#include "M2_Part_Event.h"

CM2_Part_Event::CM2_Part_Event(const CM2& M2Object, const std::shared_ptr<IFile>& File, const SM2_Event& M2Event)
	: m_M2Object(M2Object)
	, m_M2Event(M2Event)
{
	memcpy(m_ID, M2Event.identifier, 4);
	m_Data = M2Event.data;
	//m_Bone = m_M2Object.getSkeleton().getBoneLookup(M2Event.bone);
	m_Position = M2Event.position;
	//m_IsEnabled.Initialize(M2Event.enabled, f, global);
}

CM2_Part_Event::~CM2_Part_Event()
{
}
