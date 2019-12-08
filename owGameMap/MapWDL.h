#pragma once

#include "WDL_LowResTile.h"
#include "MapWMOInstance.h"

// FORWARD BEGIN
class CMap;
class WDL_Node_Material;
// FORWARD END

class CMapWDL 
	: public ISceneNodeProvider
{
public:
	CMapWDL(IBaseManager* BaseManager, std::weak_ptr<const CMap> MapController);
	virtual ~CMapWDL();

	// ISceneNodeProvider
	void CreateInsances(std::weak_ptr<ISceneNode> _parent) override;

	void UpdateCamera(const ICamera* camera);

	void Load();

	std::shared_ptr<ITexture> getMinimap() { return m_Minimap; }

private:
	std::shared_ptr<ITexture>					    m_Minimap;
	uint32											m_MAREOffsets[C_TilesInMap][C_TilesInMap];
	std::shared_ptr<WDL_Node_Material>              m_LowResilutionTileMaterial;
	std::vector<std::string>						m_LowResolutionWMOsNames;
	std::vector<ADT_MODF>					        m_LowResolutionWMOsPlacementInfo;
	std::vector<std::shared_ptr<CMapWMOInstance>>	m_LowResolutionWMOs;

private: // PARENT
	const std::weak_ptr<const CMap>					m_MapController;

private:
	IBaseManager*                                   m_BaseManager;
};