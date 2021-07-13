#pragma once

#include "Instances/MapWMOInstance.h"

// FORWARD BEGIN
class CMap;
// FORWARD END

class CMapWDL 
	: public ISceneNodeProvider
{
public:
	CMapWDL(const CMap& Map);
	virtual ~CMapWDL();

	// ISceneNodeProvider
	void CreateInsances(const std::shared_ptr<ISceneNode>& Parent) const override;

	void Load();

	std::shared_ptr<ITexture> getMinimap() { return m_Minimap; }

private:
	const CMap&                                     m_Map;
	std::shared_ptr<ITexture>					    m_Minimap;
	uint32											m_MAREOffsets[C_TilesInMap][C_TilesInMap];
	std::vector<std::string>						m_LowResolutionWMOsNames;
#ifdef USE_WMO_MODELS
	std::vector<SMapTile_MODF>					        m_LowResolutionWMOsPlacementInfo;
	mutable std::vector<CMapWMOInstance*>	        m_LowResolutionWMOs;
#endif	
};