#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWWorldObject.h"

CWoWWorldObject::CWoWWorldObject(IScene& Scene, CWoWObjectGuid Guid)
	: WoWObject(Guid)
	, m_HiddenNodeDirty(false)
	, m_BaseManager(Scene.GetBaseManager())
{
}

CWoWWorldObject::~CWoWWorldObject()
{
}

void CWoWWorldObject::Update(const UpdateEventArgs & e)
{
	if (m_HiddenNodeDirty && m_HiddenNode)
	{
		m_HiddenNode->SetLocalPosition(Position);
		m_HiddenNode->SetLocalRotationEuler(glm::vec3(0.0f, Orientation, 0.0f));
		m_HiddenNodeDirty = false;
	}
}

void CWoWWorldObject::CommitPositionAndRotation()
{
	m_HiddenNodeDirty = true;
}

#endif
