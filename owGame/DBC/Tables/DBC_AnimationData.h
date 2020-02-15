#pragma once

#include "DBC/DBC__File.h"

class CDBCStorage;

DBC_DEF_BEGIN(DBC_AnimationData)

__DBC_STRING(							Name,				2);
__DBC_TVALUE(uint32,					WeaponFlags,		3);
__DBC_TVALUE(uint32,					BodyFlags,			4);
__DBC_TVALUE(uint32,					Flags,				5);

__DBC_REF_ID(DBC_AnimationData,			Fallback,			6);
__DBC_REF_ID(DBC_AnimationData,			Previos,			7);

DBC_DEF_END