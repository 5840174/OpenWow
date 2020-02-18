#pragma once

#include "Map/Instances/MapWMOInstance.h"
#include "Map/WDL_LowResTile.h"


// FORWARD BEGIN
class CMap;
class WDL_Node_Material;
// FORWARD END

class CMapWDL 
	: public ISceneNodeProvider
{
public:
	CMapWDL(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const CMap& Map);
	virtual ~CMapWDL();

	// ISceneNodeProvider
	void CreateInsances(ISceneNode3D* _parent) const override;

	void UpdateCamera(const ICameraComponent3D* camera);

	void Load();

	std::shared_ptr<ITexture> getMinimap() { return m_Minimap; }

private:
	std::shared_ptr<ITexture>					    m_Minimap;
	uint32											m_MAREOffsets[C_TilesInMap][C_TilesInMap];
	mutable std::shared_ptr<WDL_Node_Material>              m_LowResilutionTileMaterial;
	std::vector<std::string>						m_LowResolutionWMOsNames;
	std::vector<ADT_MODF>					        m_LowResolutionWMOsPlacementInfo;
	mutable std::vector<CMapWMOInstance*>	m_LowResolutionWMOs;

private: // PARENT
	IBaseManager& m_BaseManager;
	IRenderDevice& m_RenderDevice;
	const CMap& m_MapController;
};