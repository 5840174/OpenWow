#include "stdafx.h"

// General
#include "DBC__Storage.h"

// All
DBC_LOAD(DBC_AnimationData,				"AnimationData.dbc")
DBC_LOAD(DBC_GroundEffectDoodad,		"GroundEffectDoodad.dbc")
DBC_LOAD(DBC_GroundEffectTexture,		"GroundEffectTexture.dbc")
DBC_LOAD(DBC_LoadingScreen,				"LoadingScreens.dbc")
DBC_LOAD(DBC_Material,					"Material.dbc")
DBC_LOAD(DBC_Map,						"Map.dbc")
DBC_LOAD(DBC_TerrainType,				"TerrainType.dbc")
DBC_LOAD(DBC_WMOAreaTable,				"WMOAreaTable.dbc")
DBC_LOAD(DBC_WorldSafeLocs,				"WorldSafeLocs.dbc")

// Achivement

// Area
DBC_LOAD(DBC_AreaTable, "AreaTable.dbc")

// Character
DBC_LOAD(DBC_CharacterFacialHairStyles,	"CharacterFacialHairStyles.dbc")
DBC_LOAD(DBC_CharComponentTextureLayouts, "CharComponentTextureLayouts.dbc")
DBC_LOAD(DBC_CharComponentTextureSections, "CharComponentTextureSections.dbc")
DBC_LOAD(DBC_CharHairGeosets,	"CharHairGeosets.dbc")
DBC_LOAD(DBC_CharSections,	"CharSections.dbc")
DBC_LOAD(DBC_ChrRaces,	"ChrRaces.dbc")

// Cinematic
DBC_LOAD(DBC_CinematicCamera, "CinematicCamera.dbc")
DBC_LOAD(DBC_CinematicSequences, "CinematicSequences.dbc")

// Creature
DBC_LOAD(DBC_CreatureDisplayInfo, "CreatureDisplayInfo.dbc")
DBC_LOAD(DBC_CreatureDisplayInfoExtra, "CreatureDisplayInfoExtra.dbc")
DBC_LOAD(DBC_CreatureModelData, "CreatureModelData.dbc")

// Item
DBC_LOAD(DBC_HelmetGeosetVisData, "HelmetGeosetVisData.dbc")
DBC_LOAD(DBC_ItemBagFamily, "ItemBagFamily.dbc")
DBC_LOAD(DBC_ItemClass, "ItemClass.dbc")
DBC_LOAD(DBC_ItemDisplayInfo, "ItemDisplayInfo.dbc")
DBC_LOAD(DBC_ItemVisualEffects, "ItemVisualEffects.dbc")
DBC_LOAD(DBC_ItemVisuals, "ItemVisuals.dbc")

// Light
DBC_LOAD(DBC_Light, "Light.dbc")
DBC_LOAD(DBC_LightFloatBand, "LightFloatBand.dbc")
DBC_LOAD(DBC_LightIntBand, "LightIntBand.dbc")
DBC_LOAD(DBC_LightParams, "LightParams.dbc")
DBC_LOAD(DBC_LightSkybox, "LightSkybox.dbc")

// Liquid
DBC_LOAD(DBC_LiquidType, "LiquidType.dbc")


void OpenDBs(std::shared_ptr<IBaseManager> BaseManager)
{
	std::shared_ptr<IFilesManager> filesManager = GetManager<IFilesManager>(BaseManager);

	// All
	DBC_AnimationData.Open(filesManager);
    DBC_GroundEffectDoodad.Open(filesManager);
    DBC_GroundEffectTexture.Open(filesManager);
    DBC_LoadingScreen.Open(filesManager);
	DBC_Material.Open(filesManager);
    DBC_Map.Open(filesManager);
	DBC_TerrainType.Open(filesManager);
    DBC_WMOAreaTable.Open(filesManager);
	DBC_WorldSafeLocs.Open(filesManager);
		
	// Achivement


	// Area
	DBC_AreaTable.Open(filesManager);

	// Character
	DBC_CharacterFacialHairStyles.Open(filesManager);
	DBC_CharComponentTextureLayouts.Open(filesManager);
	DBC_CharComponentTextureSections.Open(filesManager);
	DBC_CharHairGeosets.Open(filesManager);
	DBC_CharSections.Open(filesManager);
	DBC_ChrRaces.Open(filesManager);

	// Cinematic
	DBC_CinematicCamera.Open(filesManager);
	DBC_CinematicSequences.Open(filesManager);

	// Creature
	DBC_CreatureDisplayInfo.Open(filesManager);
	DBC_CreatureDisplayInfoExtra.Open(filesManager);
	DBC_CreatureModelData.Open(filesManager);

	// Item
	DBC_HelmetGeosetVisData.Open(filesManager);
	DBC_ItemBagFamily.Open(filesManager);
	DBC_ItemClass.Open(filesManager);
	DBC_ItemDisplayInfo.Open(filesManager);
	DBC_ItemVisualEffects.Open(filesManager);
	DBC_ItemVisuals.Open(filesManager);

	// Light
	DBC_Light.Open(filesManager);
	DBC_LightFloatBand.Open(filesManager);
	DBC_LightIntBand.Open(filesManager);
	DBC_LightParams.Open(filesManager);
	DBC_LightSkybox.Open(filesManager);

	// Liquid
	DBC_LiquidType.Open(filesManager);
}
