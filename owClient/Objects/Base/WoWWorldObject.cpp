#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWWorldObject.h"

// Additional
#include "../../World/World.h"

CWoWWorldObject::CWoWWorldObject(IScene& Scene, CWoWWorld& WoWWorld, CWoWGuid Guid)
	: WoWObject(Guid)
	, m_Scene(Scene)
	, m_WoWWorld(WoWWorld)
	, m_BaseManager(Scene.GetBaseManager())

	, m_IsTranslationDirty(false)
{}

CWoWWorldObject::~CWoWWorldObject()
{}

void CWoWWorldObject::Update(const UpdateEventArgs & e)
{
	if (m_IsTranslationDirty)
	{
		//if (TransportID != 0)
		//{
		//	if (auto lockedTransportObject = TransportObject.lock())
		//	{
				//m_HiddenNode->SetLocalPosition(/*lockedTransportObject->Position +*/ PositionTransportOffset);
				//m_HiddenNode->SetLocalRotationEuler(glm::vec3(0.0f, /*lockedTransportObject->Orientation*/ + OrientationTransportOffset, 0.0f));
				//m_IsTranslationDirty = false;
		//	}
		//	else
		//	{
		//		auto transportNode = GetWoWWorld().GetWorldObjects().GetWoWObject(TransportID);
		//		TransportObject = std::dynamic_pointer_cast<WoWGameObjectMOTransport>(transportNode);
		//	}
		//}
		//else
		{
			OnHiddenNodePositionChanged();
			m_IsTranslationDirty = false;
		}
	}
}

void CWoWWorldObject::OnHiddenNodePositionChanged()
{}

void CWoWWorldObject::CommitPositionAndRotation()
{
	m_IsTranslationDirty = true;
}

#endif
