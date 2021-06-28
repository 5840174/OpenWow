#pragma once

#include "Instances/MapWMOInstance.h"

// FORWARD BEGIN
class CMap;
// FORWARD END

struct WDT_MPHD
{
	struct Flags
	{
		uint32 Flag_GlobalWMO : 1;  // Use global map object definition.
		uint32 Flag_UseMCCV : 1;    // Use vertex shading (ADT.MCNK.MCCV)
		uint32 Flag_8bitMCAL : 1;   // Decides whether to use _env terrain shaders or not: funky and if MCAL has 4096 instead of 2048(?)
		uint32 Flag_HasDoodadRefsSortedBySizeCat : 1;       // if enabled, the ADT's MCRF(m2 only) chunks need to be sorted by size category

#if WOW_CLIENT_VERSION == WOW_CATA_4_3_4
		uint32 Flag_HasMCLV : 1;    // vertexBufferFormat = PNC2. (adds second color: ADT.MCNK.MCLV)
		uint32 Flag_FlipGround : 1; // Flips the ground display upside down to create a ceiling (Cataclysm)
		uint32 : 26;
#else
		uint32 : 28;
#endif

	} flags;
	uint32 something;
	uint32 unused[6];
};

struct WDT_MAIN
{
	struct Flags
	{
		uint32 Flag_HasADT : 1;
		uint32 Flag_IsLoaded : 1;
		uint32 : 30;
	} flags;
	uint32_t asyncId;
};

class CMapWDT 
	: public ISceneNodeProvider
{
public:
	CMapWDT(const CMap& Map);
	virtual ~CMapWDT();

	// ISceneNodeProvider
	void CreateInsances(const std::shared_ptr<ISceneNode>& Parent) const override;

	void Load();

	bool IsMapTileExists() { return m_IsTileBased; }
	bool IsMapTileUse8BitAlphaMCAL() const { return m_MPHD.flags.Flag_8bitMCAL; }

	bool MapHasGlobalWMO() { return false == m_GlobalWMOName.empty(); }
#ifdef USE_WMO_MODELS
	std::shared_ptr<CMapWMOInstance> GetGlobalWMOInstance() const { return m_GlobalWMO; }
#endif


private:
	const CMap&                         m_Map;

	WDT_MPHD						    m_MPHD;
	bool								m_IsTileBased;
	WDT_MAIN							m_TileFlag[C_TilesInMap][C_TilesInMap];

	// Global WMO
	std::string							m_GlobalWMOName;
#ifdef USE_WMO_MODELS
	SMapTile_MODF							m_GlobalWMOPlacementInfo;
	std::shared_ptr<CMapWMOInstance>	m_GlobalWMO;
#endif
};