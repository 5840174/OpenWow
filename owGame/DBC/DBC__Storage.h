#pragma once

// My files
#include "Tables/DBC_AnimationData.h"
#include "Tables/DBC_GroundEffectDoodad.h"
#include "Tables/DBC_GroundEffectTexture.h"
#include "Tables/DBC_LoadingScreen.h"
#include "Tables/DBC_Material.h"
#if WOW_CLIENT_VERSION < WOW_WOTLK_3_3_5
#include "Tables/DBC_Map.h"
#else 
#include "Tables/DBC_Map_WotLK.h"
#endif
#include "Tables/DBC_TerrainType.h"
#include "Tables/DBC_WMOAreaTable.h"
#include "Tables/DBC_WorldSafeLocs.h"

// Achivement

// Area
#include "Tables/DBC_AreaTable.h"

// Character
#include "Tables/DBC_CharacterFacialHairStyles.h"
#include "Tables/DBC_CharComponentTextureLayouts.h"
#include "Tables/DBC_CharComponentTextureSections.h"
#include "Tables/DBC_CharHairGeosets.h"
#include "Tables/DBC_CharSections.h"
#include "Tables/DBC_ChrRaces.h"

// Cinematic
#include "Tables/DBC_CinematicCamera.h"
#include "Tables/DBC_CinematicSequences.h"

// Creature
#include "Tables/DBC_CreatureDisplayInfo.h"
#include "Tables/DBC_CreatureDisplayInfoExtra.h"
#include "Tables/DBC_CreatureModelData.h"

// Item
#include "Tables/DBC_HelmetGeosetVisData.h"
#include "Tables/DBC_Item.h"
#include "Tables/DBC_ItemBagFamily.h"
#include "Tables/DBC_ItemClass.h"
#include "Tables/DBC_ItemDisplayInfo.h"
#include "Tables/DBC_ItemVisualEffects.h"
#include "Tables/DBC_ItemVisuals.h"

// Light
#include "Tables/DBC_Light.h"
#include "Tables/DBC_LightFloatBand.h"
#include "Tables/DBC_LightIntBand.h"
#include "Tables/DBC_LightParams.h"
#include "Tables/DBC_LightSkyBox.h"

// Liquid
#include "Tables/DBC_LiquidType.h"

// GameObject
#include "Tables/DBC_GameObjectDisplayInfo.h"


class ZN_API __declspec(uuid("1E65D52E-5137-4B9E-8B3A-9D111F398742")) CDBCStorage
	: public IManager
{
public:
	CDBCStorage(IBaseManager& BaseManager);
	virtual ~CDBCStorage();

public:
	// All
	DBC_DEFINE(DBC_AnimationData);
	DBC_DEFINE(DBC_GroundEffectDoodad);
	DBC_DEFINE(DBC_GroundEffectTexture);
	DBC_DEFINE(DBC_LoadingScreen);
	DBC_DEFINE(DBC_Material);
	DBC_DEFINE(DBC_Map);
	DBC_DEFINE(DBC_TerrainType);
	DBC_DEFINE(DBC_WMOAreaTable);
	DBC_DEFINE(DBC_WorldSafeLocs);

	// Achivement


	// Area
	DBC_DEFINE(DBC_AreaTable);

	// Character
	DBC_DEFINE(DBC_CharacterFacialHairStyles)
	DBC_DEFINE(DBC_CharComponentTextureLayouts)
	DBC_DEFINE(DBC_CharComponentTextureSections)
	DBC_DEFINE(DBC_CharHairGeosets)
	DBC_DEFINE(DBC_CharSections)
	DBC_DEFINE(DBC_ChrRaces)

	// Cinematic
	DBC_DEFINE(DBC_CinematicCamera)
	DBC_DEFINE(DBC_CinematicSequences)

	// Creatures
	DBC_DEFINE(DBC_CreatureDisplayInfo)
	DBC_DEFINE(DBC_CreatureDisplayInfoExtra)
	DBC_DEFINE(DBC_CreatureModelData)

	// Item
	DBC_DEFINE(DBC_HelmetGeosetVisData)
	DBC_DEFINE(DBC_Item)
	DBC_DEFINE(DBC_ItemBagFamily)
	DBC_DEFINE(DBC_ItemClass)
	DBC_DEFINE(DBC_ItemDisplayInfo)
	DBC_DEFINE(DBC_ItemVisualEffects)
	DBC_DEFINE(DBC_ItemVisuals)

	// Light
	DBC_DEFINE(DBC_Light)
	DBC_DEFINE(DBC_LightFloatBand)
	DBC_DEFINE(DBC_LightIntBand)
	DBC_DEFINE(DBC_LightParams)
	DBC_DEFINE(DBC_LightSkybox)

	// Liquid
	DBC_DEFINE(DBC_LiquidType)

	// GameObject
	DBC_DEFINE(DBC_GameObjectDisplayInfo)

private:
	IBaseManager& m_BaseManager;
};
