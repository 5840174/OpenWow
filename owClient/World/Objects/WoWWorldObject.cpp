#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWWorldObject.h"

// Additional
#include "../World.h"
#include "WoWGameObjectMOTransport.h"

CWoWWorldObject::CWoWWorldObject(IScene& Scene, CWoWWorld& WoWWorld, CWoWObjectGuid Guid)
	: WoWObject(Guid)
	, m_Scene(Scene)
	, m_WoWWorld(WoWWorld)
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
		//if (TransportID != 0)
		//{
		//	if (auto lockedTransportObject = TransportObject.lock())
		//	{
				//m_HiddenNode->SetLocalPosition(/*lockedTransportObject->Position +*/ PositionTransportOffset);
				//m_HiddenNode->SetLocalRotationEuler(glm::vec3(0.0f, /*lockedTransportObject->Orientation*/ + OrientationTransportOffset, 0.0f));
				//m_HiddenNodeDirty = false;
		//	}
		//	else
		//	{
		//		auto transportNode = GetWoWWorld().GetWorldObjects().GetWoWObject(TransportID);
		//		TransportObject = std::dynamic_pointer_cast<WoWGameObjectMOTransport>(transportNode);
		//	}
		//}
		//else
		{
			m_HiddenNode->SetLocalPosition(Position);
			m_HiddenNode->SetLocalRotationEuler(glm::vec3(0.0f, Orientation, 0.0f));
			m_HiddenNodeDirty = false;
		}
	}
}

void CWoWWorldObject::CommitPositionAndRotation()
{
	m_HiddenNodeDirty = true;
}

#endif
