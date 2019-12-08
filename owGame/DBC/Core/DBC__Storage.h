#pragma once

#include "DBC__Forwards.h"

// My files
#include "..\\DBC_AnimationData.h"
#include "..\\DBC_GroundEffectDoodad.h"
#include "..\\DBC_GroundEffectTexture.h"
#include "..\\DBC_LoadingScreen.h"
#include "..\\DBC_Material.h"
#include "..\\DBC_Map.h"
#include "..\\DBC_TerrainType.h"
#include "..\\DBC_WMOAreaTable.h"
#include "..\\DBC_WorldSafeLocs.h"

// Achivement

// Area
#include "..\\DBC_AreaTable.h"

// Character
#include "..\\DBC_CharacterFacialHairStyles.h"
#include "..\\DBC_CharComponentTextureLayouts.h"
#include "..\\DBC_CharComponentTextureSections.h"
#include "..\\DBC_CharHairGeosets.h"
#include "..\\DBC_CharSections.h"
#include "..\\DBC_ChrRaces.h"

// Cinematic
#include "..\\DBC_CinematicCamera.h"
#include "..\\DBC_CinematicSequences.h"

// Creature
#include "..\\DBC_CreatureDisplayInfo.h"
#include "..\\DBC_CreatureDisplayInfoExtra.h"
#include "..\\DBC_CreatureModelData.h"

// Item
#include "..\\DBC_HelmetGeosetVisData.h"
//#if VERSION == VERSION_WotLK
#include "..\\DBC_Item.h"
//#endif
#include "..\\DBC_ItemBagFamily.h"
#include "..\\DBC_ItemClass.h"
#include "..\\DBC_ItemDisplayInfo.h"
#include "..\\DBC_ItemVisualEffects.h"
#include "..\\DBC_ItemVisuals.h"

// Light
#include "..\\DBC_Light.h"
#include "..\\DBC_LightFloatBand.h"
#include "..\\DBC_LightIntBand.h"
#include "..\\DBC_LightParams.h"
#include "..\\DBC_LightSkyBox.h"

// Liquid
#include "..\\DBC_LiquidType.h"


 void OpenDBs(IBaseManager* BaseManager);
