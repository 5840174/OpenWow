#include "stdafx.h"

// General
#include "DBC__Storage.h"

CDBCStorage::CDBCStorage(IBaseManager& BaseManager)
	: m_BaseManager(BaseManager)
{
	IFilesManager* filesManager = m_BaseManager.GetManager<IFilesManager>();

	// All
	DBC_LOAD(DBC_AnimationData, filesManager, "AnimationData.dbc");
	DBC_LOAD(DBC_GroundEffectDoodad, filesManager, "GroundEffectDoodad.dbc");
	DBC_LOAD(DBC_GroundEffectTexture, filesManager, "GroundEffectTexture.dbc");
	DBC_LOAD(DBC_LoadingScreen, filesManager, "LoadingScreens.dbc");
	DBC_LOAD(DBC_Material, filesManager, "Material.dbc");
	DBC_LOAD(DBC_Map, filesManager, "Map.dbc");
	DBC_LOAD(DBC_TerrainType, filesManager, "TerrainType.dbc");
	DBC_LOAD(DBC_WMOAreaTable, filesManager, "WMOAreaTable.dbc");
	DBC_LOAD(DBC_WorldSafeLocs, filesManager, "WorldSafeLocs.dbc");

	// Achivement

	// Area
	DBC_LOAD(DBC_AreaTable, filesManager, "AreaTable.dbc");

	// Character
	DBC_LOAD(DBC_CharacterFacialHairStyles, filesManager, "CharacterFacialHairStyles.dbc");
	DBC_LOAD(DBC_CharComponentTextureLayouts, filesManager, "CharComponentTextureLayouts.dbc");
	DBC_LOAD(DBC_CharComponentTextureSections, filesManager, "CharComponentTextureSections.dbc");
	DBC_LOAD(DBC_CharHairGeosets, filesManager, "CharHairGeosets.dbc");
	DBC_LOAD(DBC_CharSections, filesManager, "CharSections.dbc");
	DBC_LOAD(DBC_ChrRaces, filesManager, "ChrRaces.dbc");

	// Cinematic
	DBC_LOAD(DBC_CinematicCamera, filesManager, "CinematicCamera.dbc");
	DBC_LOAD(DBC_CinematicSequences, filesManager, "CinematicSequences.dbc");

	// Creature
	DBC_LOAD(DBC_CreatureDisplayInfo, filesManager, "CreatureDisplayInfo.dbc");
	DBC_LOAD(DBC_CreatureDisplayInfoExtra, filesManager, "CreatureDisplayInfoExtra.dbc");
	DBC_LOAD(DBC_CreatureModelData, filesManager, "CreatureModelData.dbc");

	// Item
	DBC_LOAD(DBC_HelmetGeosetVisData, filesManager, "HelmetGeosetVisData.dbc");
	DBC_LOAD(DBC_ItemBagFamily, filesManager, "ItemBagFamily.dbc");
	DBC_LOAD(DBC_ItemClass, filesManager, "ItemClass.dbc");
	DBC_LOAD(DBC_ItemDisplayInfo, filesManager, "ItemDisplayInfo.dbc");
	DBC_LOAD(DBC_ItemVisualEffects, filesManager, "ItemVisualEffects.dbc");
	DBC_LOAD(DBC_ItemVisuals, filesManager, "ItemVisuals.dbc");

	// Light
	DBC_LOAD(DBC_Light, filesManager, "Light.dbc");
	DBC_LOAD(DBC_LightFloatBand, filesManager, "LightFloatBand.dbc");
	DBC_LOAD(DBC_LightIntBand, filesManager, "LightIntBand.dbc");
	DBC_LOAD(DBC_LightParams, filesManager, "LightParams.dbc");
	DBC_LOAD(DBC_LightSkybox, filesManager, "LightSkybox.dbc");

	// Liquid
	DBC_LOAD(DBC_LiquidType, filesManager, "LiquidType.dbc");

	// GameObject
	DBC_LOAD(DBC_GameObjectDisplayInfo, filesManager, "GameObjectDisplayInfo.dbc");

	// Transport
	DBC_LOAD(DBC_TransportAnimation, filesManager, "TransportAnimation.dbc");

	// Taxi
	DBC_LOAD(DBC_TaxiNodes, filesManager, "TaxiNodes.dbc");
	DBC_LOAD(DBC_TaxiPath, filesManager, "TaxiPath.dbc");
	DBC_LOAD(DBC_TaxiPathNode, filesManager, "TaxiPathNode.dbc");
}

CDBCStorage::~CDBCStorage()
{
}
