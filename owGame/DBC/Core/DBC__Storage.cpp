#include "stdafx.h"

// General
#include "DBC__Storage.h"

// All
DBC_LOAD(DBC_AnimationData)
DBC_LOAD(DBC_GroundEffectDoodad)
DBC_LOAD(DBC_GroundEffectTexture)
DBC_LOAD(DBC_LoadingScreen)
DBC_LOAD(DBC_Material)
DBC_LOAD(DBC_Map)
DBC_LOAD(DBC_TerrainType)
DBC_LOAD(DBC_WMOAreaTable)
DBC_LOAD(DBC_WorldSafeLocs)

// Achivement

// Area
DBC_LOAD(DBC_AreaTable)

// Character
DBC_LOAD(DBC_CharacterFacialHairStyles)
DBC_LOAD(DBC_CharComponentTextureLayouts)
DBC_LOAD(DBC_CharComponentTextureSections)
DBC_LOAD(DBC_CharHairGeosets)
DBC_LOAD(DBC_CharSections)
DBC_LOAD(DBC_ChrRaces)

// Cinematic
DBC_LOAD(DBC_CinematicCamera)
DBC_LOAD(DBC_CinematicSequences)

// Creature
DBC_LOAD(DBC_CreatureDisplayInfo)
DBC_LOAD(DBC_CreatureDisplayInfoExtra)
DBC_LOAD(DBC_CreatureModelData)

// Item
DBC_LOAD(DBC_HelmetGeosetVisData)
DBC_LOAD(DBC_ItemBagFamily)
DBC_LOAD(DBC_ItemClass)
DBC_LOAD(DBC_ItemDisplayInfo)
DBC_LOAD(DBC_ItemVisualEffects)
DBC_LOAD(DBC_ItemVisuals)

// Light
DBC_LOAD(DBC_Light)
DBC_LOAD(DBC_LightFloatBand)
DBC_LOAD(DBC_LightIntBand)
DBC_LOAD(DBC_LightParams)
DBC_LOAD(DBC_LightSkybox)

// Liquid
DBC_LOAD(DBC_LiquidType)


void OpenDBs(IBaseManager* BaseManager)
{
	IFilesManager* filesManager = BaseManager->GetManager<IFilesManager>();

	// All
	DBC_AnimationData.Open(filesManager, "AnimationData.dbc");
	DBC_GroundEffectDoodad.Open(filesManager, "GroundEffectDoodad.dbc");
	DBC_GroundEffectTexture.Open(filesManager, "GroundEffectTexture.dbc");
	DBC_LoadingScreen.Open(filesManager, "LoadingScreens.dbc");
	DBC_Material.Open(filesManager, "Material.dbc");
	DBC_Map.Open(filesManager, "Map.dbc");
	DBC_TerrainType.Open(filesManager, "TerrainType.dbc");
	DBC_WMOAreaTable.Open(filesManager, "WMOAreaTable.dbc");
	DBC_WorldSafeLocs.Open(filesManager, "WorldSafeLocs.dbc");

	// Achivement

	// Area
	DBC_AreaTable.Open(filesManager, "AreaTable.dbc");

	// Character
	DBC_CharacterFacialHairStyles.Open(filesManager, "CharacterFacialHairStyles.dbc");
	DBC_CharComponentTextureLayouts.Open(filesManager, "CharComponentTextureLayouts.dbc");
	DBC_CharComponentTextureSections.Open(filesManager, "CharComponentTextureSections.dbc");
	DBC_CharHairGeosets.Open(filesManager, "CharHairGeosets.dbc");
	DBC_CharSections.Open(filesManager, "CharSections.dbc");
	DBC_ChrRaces.Open(filesManager, "ChrRaces.dbc");

	// Cinematic
	DBC_CinematicCamera.Open(filesManager, "CinematicCamera.dbc");
	DBC_CinematicSequences.Open(filesManager, "CinematicSequences.dbc");

	// Creature
	DBC_CreatureDisplayInfo.Open(filesManager, "CreatureDisplayInfo.dbc");
	DBC_CreatureDisplayInfoExtra.Open(filesManager, "CreatureDisplayInfoExtra.dbc");
	DBC_CreatureModelData.Open(filesManager, "CreatureModelData.dbc");

	// Item
	DBC_HelmetGeosetVisData.Open(filesManager, "HelmetGeosetVisData.dbc");
	DBC_ItemBagFamily.Open(filesManager, "ItemBagFamily.dbc");
	DBC_ItemClass.Open(filesManager, "ItemClass.dbc");
	DBC_ItemDisplayInfo.Open(filesManager, "ItemDisplayInfo.dbc");
	DBC_ItemVisualEffects.Open(filesManager, "ItemVisualEffects.dbc");
	DBC_ItemVisuals.Open(filesManager, "ItemVisuals.dbc");

	// Light
	DBC_Light.Open(filesManager, "Light.dbc");
	DBC_LightFloatBand.Open(filesManager, "LightFloatBand.dbc");
	DBC_LightIntBand.Open(filesManager, "LightIntBand.dbc");
	DBC_LightParams.Open(filesManager, "LightParams.dbc");
	DBC_LightSkybox.Open(filesManager, "LightSkybox.dbc");

	// Liquid
	DBC_LiquidType.Open(filesManager, "LiquidType.dbc");
}
