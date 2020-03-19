#pragma once

#include "DBC/DBC__File.h"

DBC_DEF_BEGIN(DBC_Map)

__DBC_STRING(							Directory,				2);		// std::string
__DBC_TVALUE(uint32,					InstanceType,			3);		// uint (0-none, 1-party, 2-raid, 4-arena, >=5-none)
__DBC_TVALUE(uint32,					MapType,				4);		// uint (1-normal, 2-test, 3-internal)
__DBC_LOCSTR(							Name,					5);
__DBC_TVALUE(uint32_t,					MinLevel,				14);
__DBC_TVALUE(uint32_t,					MaxLevel,				15);
__DBC_TVALUE(uint32_t,					MaxPlayers,				16);

// 9 - ?
// 10 - ?
// 11 - ?

__DBC_REF_ID(							DBC_Map, parentMapID,	20);
__DBC_LOCSTR(							MapDescriptionAllicane, 21 /* + 8 = 29*/);
__DBC_LOCSTR(							MapDescriptionHorde,	30);

__DBC_REF_ID(DBC_LoadingScreen,			LoadingScreen,			39);

DBC_DEF_END