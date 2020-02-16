#pragma once

#include "WMO_Group_Headers.h"

// Parts
#include "WMO_Liquid.h"
#include "WMO_Part_Portal.h"

#include "WMO_Group_Part_Batch.h"
#include "WMO_Group_Part_BSP_Node.h"

// FORWARD BEGIN
class CWMO;
class CWMO_Group_Instance;
class CWMO_Doodad_Instance;
// FORWARD END

class WMO_Group 
{
public:
	WMO_Group(IBaseManager* BaseManager, IRenderDevice& RenderDevice, const CWMO& WMOModel, const uint32 GroupIndex, const SWMO_GroupInfoDef& GroupProto);
	virtual ~WMO_Group();

	void CreateInsances(CWMO_Group_Instance* _parent) const;

	uint32 to_wmo_liquid(int x);

	void Load();

	const CWMO& GetWMOModel() const { return m_WMOModel; }

public:
	std::string                             m_GroupName;
	
	SWMO_Group_HeaderDef					m_Header;
	BoundingBox								m_Bounds;

	std::vector< std::shared_ptr<CWMO_Part_Portal>>				m_Portals;

public:
	//-- Triangles --//
	vec3 * dataFromMOVT;
	std::vector<SWMO_Group_MaterialDef>		m_MaterialsInfo;
	bool									m_IsMOCVExists;

	//-- Render bathes --//
	std::vector<SWMO_Group_BatchDef>		m_WMOBatchs;
	std::vector<std::shared_ptr<WMO_Group_Part_Batch>>		m_WMOBatchIndexes;
	SWMO_Group_BatchDef*					moba;

	//-- Lights --//
	std::vector<uint16>						m_WMOLightsIndexes;
	C4Vec*									mocv;
	uint32									mocv_count;

	//-- Doodads references --//
	std::vector<uint16>						m_DoodadsPlacementIndexes;

	//-- Collision --//
	// MOBN chunk
	// MOBR chunk
	std::vector<uint16>                     collisionIndexes;
	std::shared_ptr<IBuffer>                 VB_Collision;
	std::vector<std::shared_ptr<CWMO_Group_Part_BSP_Node>>	m_CollisionNodes;

	//-- Liquid --//
	SWMO_Group_MLIQDef						m_LiquidHeader;
	std::shared_ptr<CWMO_Liquid>            m_WMOLiqiud;
	

private:
	IBaseManager* m_BaseManager;
	IRenderDevice& m_RenderDevice;
	const CWMO& m_WMOModel;
	const uint32 m_GroupIndex;
};