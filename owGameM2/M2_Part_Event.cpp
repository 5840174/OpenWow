#include "stdafx.h"

// Include
#include "M2.h"

// General
#include "M2_Part_Event.h"

CM2_Part_Event::CM2_Part_Event(const std::weak_ptr<M2> _parentM2, std::shared_ptr<IFile> f, const SM2_Event& _proto, cGlobalLoopSeq global)
{
	const std::shared_ptr<M2> ParentM2 = _parentM2.lock();
	_ASSERT(ParentM2 != nullptr);

	memcpy(m_ID, _proto.identifier, 4);
	m_Data = _proto.data;
	m_Bone = ParentM2->getSkeleton()->getBoneLookup(_proto.bone);
	m_Position = _proto.position;
	//m_IsEnabled.init(_proto.enabled, f, global);
}
