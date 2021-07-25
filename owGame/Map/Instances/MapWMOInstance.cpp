#include "stdafx.h"

#ifdef USE_WMO_MODELS

// General
#include "MapWMOInstance.h"

// Additional
#include "../MapTile.h"

CMapWMOInstance::CMapWMOInstance(IScene& Scene, CWorldClient& WorldClient, const std::shared_ptr<CWMO>& WMOObject, const SMapTile_MODF& MODF)
	: CWMO_Base_Instance(Scene, WorldClient, WMOObject)
	, m_PlacementInfo(MODF)
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

	SetCullDistance(GetBaseManager().GetManager<ISettings>()->GetGroup("WoWSettings")->GetPropertyT<float>("MapTileWMORenderDistance")->Get());
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

	__super::Accept(visitor);
}



//
// ILoadable
//
bool CMapWMOInstance::Load()
{
	if (false == __super::Load())
		return false;

	if (auto parent = GetParent())
	{
		if (auto parentAsMapTile = std::dynamic_pointer_cast<CMapTile>(parent))
		{
			parentAsMapTile->ExtendMapTileBounds(GetWorldBounds());
		}
	}
	
	return true;
}


//
// Protected
//
glm::mat4 CMapWMOInstance::CalculateLocalTransform() const
{
	glm::mat4 localTransform = glm::mat4(1.0f);
	localTransform = glm::translate(localTransform, m_PlacementInfo.position);
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