#pragma once

#include "DBC/DBC__File.h"

DBC_DEF_BEGIN(DBC_CreatureDisplayInfoExtra)

__DBC_REF_ID(DBC_ChrRaces,               Race,              2);
__DBC_TVALUE(uint32,                     Gender,            3);

__DBC_TVALUE(uint32,                     SkinID,            4);
__DBC_TVALUE(uint32,                     FaceID,            5);
__DBC_TVALUE(uint32,                     HairStyleID,       6);
__DBC_TVALUE(uint32,                     HairColorID,       7);
__DBC_TVALUE(uint32,                     FacialHairID,      8);

__DBC_REF_ID(DBC_ItemDisplayInfo,        Helm,              9);
__DBC_REF_ID(DBC_ItemDisplayInfo,        Shoulder,          10);
__DBC_REF_ID(DBC_ItemDisplayInfo,        Shirt,             11);
__DBC_REF_ID(DBC_ItemDisplayInfo,        Chest,             12);
__DBC_REF_ID(DBC_ItemDisplayInfo,        Belt,              13);
__DBC_REF_ID(DBC_ItemDisplayInfo,        Legs,              14);
__DBC_REF_ID(DBC_ItemDisplayInfo,        Boots,             15);
__DBC_REF_ID(DBC_ItemDisplayInfo,        Wrist,             16);
__DBC_REF_ID(DBC_ItemDisplayInfo,        Gloves,            17);
__DBC_REF_ID(DBC_ItemDisplayInfo,        Tabard,            18);

__DBC_STRING(                            BakedSkin,         19);

DBC_DEF_END
