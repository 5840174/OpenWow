#include "stdafx.h"

#ifdef USE_WMO_MODELS

// General
#include "MapWMOInstance.h"

CMapWMOInstance::CMapWMOInstance(IScene& Scene, const std::shared_ptr<CWMO>& WMOObject, const ADT_MODF& _placementInfo)
	: CWMO_Base_Instance(Scene, WMOObject)
	, m_PlacementInfo(_placementInfo)
{
	SetName("CMapWMOInstance: " + WMOObject->GetFilename());
}

CMapWMOInstance::~CMapWMOInstance()
{
	//Log::Info("CMapWMOInstance: '%s' deleted.", GetName().c_str());
}



//
// CWMO_Base_Instance
//
uint16 CMapWMOInstance::GetDoodadSetIndex() const
{
	return m_PlacementInfo.doodadSetIndex;
}

//
// ISceneNode
//
void CMapWMOInstance::Initialize()
{
	__super::Initialize();

	uint16 doodadSetIndex = m_PlacementInfo.doodadSetIndex;
	//m_DoodadSetInfo = _wmoObject->m_DoodadsSetInfos[doodadSetIndex];

	SetLocalPosition(m_PlacementInfo.position);
	SetLocalRotationEuler(m_PlacementInfo.rotation);
}

void CMapWMOInstance::Accept(IVisitor* visitor)
{
	auto& idIter = m_AlreadyDraw[m_PlacementInfo.uniqueId];
	if (idIter == nullptr)
		return;
	idIter = this;

	CWMO_Base_Instance::Accept(visitor);
}


//
// Protected
//
glm::mat4 CMapWMOInstance::CalculateLocalTransform() const
{
	glm::mat4 localTransform = glm::mat4(1.0f);
	localTransform = glm::translate(localTransform, GetLocalPosition());
	localTransform *= glm::eulerAngleYZX(glm::radians(m_PlacementInfo.rotation.y - 90.0f), glm::radians(-m_PlacementInfo.rotation.x), glm::radians(m_PlacementInfo.rotation.z));
	return localTransform;
}

//

void CMapWMOInstance::reset()
{
	m_AlreadyDraw.clear();
}
std::map<uint32, const CMapWMOInstance*> CMapWMOInstance::m_AlreadyDraw;

#endif