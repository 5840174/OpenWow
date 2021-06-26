#pragma once

#include "Instances/MapWMOInstance.h"

// FORWARD BEGIN
class CMap;
class CMapWDLTileMaterial;
// FORWARD END

class CMapWDL 
	: public ISceneNodeProvider
{
public:
	CMapWDL(const CMap& Map);
	virtual ~CMapWDL();

	// ISceneNodeProvider
	void CreateInsances(const std::shared_ptr<ISceneNode>& Parent) const override;

	void UpdateCamera(const ICameraComponent3D* camera);

	void Load();

	std::shared_ptr<ITexture> getMinimap() { return m_Minimap; }

private:
	const CMap&                                     m_Map;
	std::shared_ptr<ITexture>					    m_Minimap;
	uint32											m_MAREOffsets[C_TilesInMap][C_TilesInMap];
	mutable std::shared_ptr<CMapWDLTileMaterial>    m_LowResilutionTileMaterial;
	std::vector<std::string>						m_LowResolutionWMOsNames;
#ifdef USE_WMO_MODELS
	std::vector<ADT_MODF>					        m_LowResolutionWMOsPlacementInfo;
	mutable std::vector<CMapWMOInstance*>	        m_LowResolutionWMOs;
#endif	
};