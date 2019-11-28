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

class WMO_Group : public std::enable_shared_from_this<WMO_Group>
{
public:
	WMO_Group(const std::weak_ptr<const CWMO> _parentWMO, const uint32 m_GroupIndex, std::string _groupName, std::shared_ptr<IFile> _groupFile);

	void CreateInsances(std::weak_ptr<CWMO_Group_Instance> _parent) const;

	uint32 to_wmo_liquid(int x);

	void Load();
	void initLighting();

	void Render(cmat4 _world) const;
	void RenderCollision(cmat4 _world) const;

public:
	const std::string                       m_GroupName;
	const std::string                       m_GroupDescription;
	const uint32							m_GroupIndex;
	std::shared_ptr<IFile>					m_F;
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

	// Parent
	const std::weak_ptr<const CWMO>          m_ParentWMO;
	const CGroupQuality*					m_Quality;
};