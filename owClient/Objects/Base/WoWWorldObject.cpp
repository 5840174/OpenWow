#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWWorldObject.h"

// Additional
#include "../../World/WorldServer.h"

CowServerWorldObject::CowServerWorldObject(IScene& Scene, CWorldServer& WoWWorld, CowGuid Guid)
	: CowServerObject(Guid)
	, m_Scene(Scene)
	, m_ServerWorld(WoWWorld)
	, m_BaseManager(Scene.GetBaseManager())

	, m_IsTranslationDirty(false)
{}

CowServerWorldObject::~CowServerWorldObject()
{}

void CowServerWorldObject::Update(const UpdateEventArgs & e)
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
		//		auto transportNode = GetWoWWorld().GetWorldObjects().Get(TransportID);
		//		TransportObject = std::dynamic_pointer_cast<CowServerGameObject_MOTransport>(transportNode);
		//	}
		//}
		//else
		{
			OnHiddenNodePositionChanged();
			m_IsTranslationDirty = false;
		}
	}
}

void CowServerWorldObject::OnHiddenNodePositionChanged()
{}

void CowServerWorldObject::CommitPositionAndRotation()
{
	m_IsTranslationDirty = true;
}

#endif
