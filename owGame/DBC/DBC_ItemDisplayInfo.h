#pragma once

#include "DBC/Core/DBC__File.h"

DBC_DEF_BEGIN(DBC_ItemDisplayInfo)

__DBC_STRARR(ObjectModelName, 2, 2);
__DBC_STRARR(ObjectTextureName, 4, 2);

__DBC_STRING(Icon, 6);				

__DBC_TARRAY(uint32, GeosetGroups, 8, 3);

__DBC_TVALUE(uint32, SpellVisualID, 11);
__DBC_TVALUE(uint32, GroupSoundID, 13);

__DBC_TVALUE(uint32, HelmetGeosetMale, 14);
__DBC_TVALUE(uint32, HelmetGeosetFemale, 15);

__DBC_STRARR(TextureComponents, 16, DBC_CharComponent_Sections::ITEMS_COUNT); // !MUST! equal DBC_CharComponent_Sections enum
__DBC_STRING(UpperArmTexture, 16);
__DBC_STRING(LowerArmTexture, 17);
__DBC_STRING(HandsTexture, 18);
__DBC_STRING(UpperTorsoTexture, 19);
__DBC_STRING(LowerTorsoTexture, 20);
__DBC_STRING(UpperLegTexture, 21);
__DBC_STRING(LowerLegTexture, 22);
__DBC_STRING(FootTexture, 23);

__DBC_REF_ID(DBC_ItemVisuals, ItemVisualID, 24);

DBC_DEF_END
