#pragma once

#ifdef USE_WMO_MODELS

// OpenWoW
#include "../WoWChunkReader.h"

// WMO Group
#include "WMOGroup_Headers.h"

// Parts
#include "WMOGroup_Part_Liquid.h"
#include "WMO_Part_Portal.h"

#include "WMOGroup_Part_Batch.h"
#include "WMOGroup_Part_CollisionNode.h"

// FORWARD BEGIN
class CWMO;
class CWMO_Group_Instance;
class CWMO_Doodad_Instance;
// FORWARD END

class CWMOGroup
	: public CLoadableObject
{
public:
	CWMOGroup(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const CWMO& WMO, const uint32 GroupIndex, const SWMO_MOGI& GroupProto);
	virtual ~CWMOGroup();

	// CWMOGroup
	const SWMOGroup_MOGP& GetHeader() const;
	const uint32 GetGroupIndex() const;
	const BoundingBox& GetBoundingBox() const;
	bool IsIndoor() const;
	bool IsOutdoor() const;
	void AddPortal(const CWMO_Part_Portal& WMOPartPortal);
	const std::vector<CWMO_Part_Portal>& GetPortals() const;

	const std::shared_ptr<IGeometry>& GetCollisionGeometry() const;
	const std::vector<std::shared_ptr<CWMOGroup_Part_CollisionNode>>& GetCollisionNodes() const;

	// ISceneNodeProvider
	void CreateInsances(const std::shared_ptr<CWMO_Group_Instance>& Parent) const;

	// CLoadableObject
	bool Load() override;
	bool Delete() override;

private:
	std::string                             m_GroupName;
	SWMOGroup_MOGP					        m_GroupHeader;
	BoundingBox								m_Bounds;

private:
	//-- Triangles --//
	std::vector<SWMOGroup_MOPY>		        m_MaterialsInfo;

	//-- Render bathes --//
	std::vector<std::shared_ptr<WMO_Group_Part_Batch>> m_WMOBatchIndexes;

	//-- Lights --//
	std::vector<uint16>						m_WMOLightsIndexes;

	//-- Doodads references --//
	std::vector<uint16>						m_DoodadsPlacementIndexes;

	//-- Collision --//
	std::vector<std::shared_ptr<CWMOGroup_Part_CollisionNode>>	m_CollisionNodes; // MOBN chunk
	std::shared_ptr<IGeometry>				        m_CollisionGeom;


	//-- Liquid --//
	std::shared_ptr<CWMOGroup_Part_Liquid>            m_WMOLiqiud;
	

private:
	std::vector<CWMO_Part_Portal> m_Portals;

private:
	IBaseManager& m_BaseManager;
	IRenderDevice& m_RenderDevice;
	const CWMO& m_WMO;
	const uint32 m_GroupIndex;
	std::unique_ptr<WoWChunkReader> m_ChunkReader;
};

#endif
