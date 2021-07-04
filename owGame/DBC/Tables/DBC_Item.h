#pragma once

#include "DBC__File.h"

enum class ZN_API DBCItem_EInventoryItemType : uint32
{
	NON_EQUIP = 0,

	HEAD = 1,
	NECK = 2,
	SHOULDERS = 3,
	BODY = 4,
	CHEST = 5,
	WAIST = 6,
	LEGS = 7,
	FEET = 8,
	WRISTS = 9,
	HANDS = 10,
	FINGER = 11,
	TRINKET = 12,
	WEAPON = 13,
	SHIELD = 14,
	RANGED = 15,
	CLOAK = 16,
	TWOHWEAPON = 17,
	BAG = 18,
	TABARD = 19,
	ROBE = 20,
	WEAPONMAINHAND = 21,
	WEAPONOFFHAND = 22,
	HOLDABLE = 23,
	AMMO = 24,
	THROWN = 25,
	RANGEDRIGHT = 26,
	QUIVER = 27,
	RELIC = 28,

	__Count = 29
};

static bool isWeapon(DBCItem_EInventoryItemType InventoryType)
{
	if (InventoryType == DBCItem_EInventoryItemType::WEAPON)
		return true;

	if (InventoryType == DBCItem_EInventoryItemType::TWOHWEAPON)
		return true;

	if (InventoryType == DBCItem_EInventoryItemType::WEAPONMAINHAND)
		return true;

	if (InventoryType == DBCItem_EInventoryItemType::WEAPONOFFHAND)
		return true;

	return false;
}

DBC_DEF_BEGIN(DBC_Item)

__DBC_REF_ID(DBC_ItemClass,              ItemClassID,            2);
__DBC_TVALUE(uint32,                     ItemSubclassID,         3);
__DBC_TVALUE(int32,                      SoundSubclassOverride,  4);
__DBC_REF_ID(DBC_Material,               MaterialID,             5);
__DBC_REF_ID(DBC_ItemDisplayInfo,        DisplayInfoID,          6);
__DBC_TVALUE(DBCItem_EInventoryItemType, InventorySlot,          7);
__DBC_TVALUE(uint32,                     SheathID,               8);

DBC_DEF_END
