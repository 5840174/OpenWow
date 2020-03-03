#include "stdafx.h"

// General
#include "MapWMOInstance.h"

// Additional
#include "WMO\\RenderPass_WMO.h"

CMapWMOInstance::CMapWMOInstance(const std::shared_ptr<CWMO>& WMOObject, const ADT_MODF& _placementInfo)
	: CWMO_Base_Instance(WMOObject)
	, m_PlacementInfo(_placementInfo)
{

}

CMapWMOInstance::~CMapWMOInstance()
{
	//Log::Info("ADT_WMO Deleted");
}


void CMapWMOInstance::Initialize()
{
	uint16 doodadSetIndex = m_PlacementInfo.doodadSetIndex;
	//m_DoodadSetInfo = _wmoObject->m_DoodadsSetInfos[doodadSetIndex];

	// CTransformComponent
	{
		// Translate
		SetTranslate(m_PlacementInfo.position);

		// Rotate
		vec3 rotate = glm::radians(m_PlacementInfo.rotation);
		rotate.x = -rotate.x;
		rotate.y = rotate.y - glm::half_pi<float>();
		SetRotation(vec3(rotate.z, rotate.y, rotate.x));
	}

	__super::Initialize();
}

void CMapWMOInstance::Accept(IVisitor* visitor)
{
	const auto& idIter = m_AlreadyDraw.find(m_PlacementInfo.uniqueId);
	if (idIter != m_AlreadyDraw.end())
	{
		if (idIter->second != this)
			return;
	}
	else
	{
		m_AlreadyDraw.insert(std::make_pair(m_PlacementInfo.uniqueId, this));
	}

	CWMO_Base_Instance::Accept(visitor);
}


//

void CMapWMOInstance::reset()
{
	m_AlreadyDraw.clear();
}
std::unordered_map<uint32, const CMapWMOInstance*> CMapWMOInstance::m_AlreadyDraw;