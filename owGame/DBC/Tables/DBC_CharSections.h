#pragma once

#include "DBC/DBC__File.h"

enum class DBC_CharSections_GeneralType : uint32
{
	Skin				= 0,
	Face				= 1,
	FacialHair			= 2,
	Hair				= 3,
	Underwear			= 4
};

enum class DBC_CharSections_Flags : uint32
{
	Playable			= 0x01,
	BarberShop			= 0x02,
	DeathKnightTexture	= 0x04,
	NecromancerHuman	= 0x08
};

DBC_DEF_BEGIN(DBC_CharSections)

#if WOW_CLIENT_VERSION == WOW_CLASSIC_1_12_1
__DBC_TVALUE(uint32,						Race,			    2);
__DBC_TVALUE(uint32,						Gender,			    3);
__DBC_TVALUE(DBC_CharSections_GeneralType,	GeneralType,	    4);
__DBC_TVALUE(uint32,                        Type,               5);
__DBC_TVALUE(uint32,                        Color,              6);

__DBC_STRING(Texture1,										    7);
__DBC_STRING(Texture2,										    8);
__DBC_STRING(Texture3,										    9);
__DBC_TVALUE(DBC_CharSections_Flags,		Flags,			    10);
#else
__DBC_TVALUE(uint32, Race, 2);
__DBC_TVALUE(uint32, Gender, 3);
__DBC_TVALUE(DBC_CharSections_GeneralType, GeneralType, 4);

__DBC_STRING(Texture1, 5);
__DBC_STRING(Texture2, 6);
__DBC_STRING(Texture3, 7);

__DBC_TVALUE(DBC_CharSections_Flags, Flags, 8);

__DBC_TVALUE(uint32, Type, 9);
__DBC_TVALUE(uint32, Color, 10);



#endif

DBC_DEF_END
