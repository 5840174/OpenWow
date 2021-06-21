#pragma once

#include "DBC/DBC__File.h"

DBC_DEF_BEGIN(DBC_Map)

__DBC_STRING(						Directory,					2);		// std::string
__DBC_TVALUE(uint32,				InstanceType,				3);		// uint (0-none, 1-party, 2-raid, 4-arena, >=5-none)
__DBC_TVALUE(uint32,				MapType,					4);		// uint (1-normal, 2-test, 3-internal)

__DBC_TVALUE(uint32,                IsBattleGround,             5);
__DBC_LOCSTR(						Name,						6);

__DBC_TVALUE(uint32,                AreaTableID,                23);

__DBC_LOCSTR(						MapDescriptionAllicane,		24);
__DBC_LOCSTR(						MapDescriptionHorde,		41);

__DBC_TVALUE(uint32,				LoadingScreenID,			58);
__DBC_TVALUE(float,                 MinimapScale,               59);

__DBC_REF_ID(DBC_Map,               CorpseMap,                  60);
__DBC_TVALUE(float,                 CorpseX,                    61);
__DBC_TVALUE(float,                 CorpseY,                    62);

__DBC_TVALUE(uint32,				TimeOfDayOverride,			63);
__DBC_TVALUE(uint32,				Expansion,					64);

DBC_DEF_END