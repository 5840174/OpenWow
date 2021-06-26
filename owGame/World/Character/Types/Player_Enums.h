#pragma once

enum PlayerSlots
{
	// first slot for item stored (in any way in player m_items data)
	PLAYER_SLOT_START = 0,

	// last+1 slot for item stored (in any way in player m_items data)
	PLAYER_SLOT_END = 150,
	PLAYER_SLOTS_COUNT = (PLAYER_SLOT_END - PLAYER_SLOT_START)
};

#define INVENTORY_SLOT_BAG_0    255

enum EquipmentSlots : uint8                                 // 19 slots
{
	EQUIPMENT_SLOT_START = 0,

	EQUIPMENT_SLOT_HEAD = 0,
	EQUIPMENT_SLOT_NECK = 1,
	EQUIPMENT_SLOT_SHOULDERS = 2,
	EQUIPMENT_SLOT_BODY = 3,
	EQUIPMENT_SLOT_CHEST = 4,
	EQUIPMENT_SLOT_WAIST = 5,
	EQUIPMENT_SLOT_LEGS = 6,
	EQUIPMENT_SLOT_FEET = 7,
	EQUIPMENT_SLOT_WRISTS = 8,
	EQUIPMENT_SLOT_HANDS = 9,
	EQUIPMENT_SLOT_FINGER1 = 10,
	EQUIPMENT_SLOT_FINGER2 = 11,
	EQUIPMENT_SLOT_TRINKET1 = 12,
	EQUIPMENT_SLOT_TRINKET2 = 13,
	EQUIPMENT_SLOT_BACK = 14,
	EQUIPMENT_SLOT_MAINHAND = 15,
	EQUIPMENT_SLOT_OFFHAND = 16,
	EQUIPMENT_SLOT_RANGED = 17,
	EQUIPMENT_SLOT_TABARD = 18,
	EQUIPMENT_SLOT_END = 19
};

enum InventorySlots : uint8                                 // 4 slots
{
	INVENTORY_SLOT_BAG_START = 19,
	INVENTORY_SLOT_BAG_END = 23
};

enum InventoryPackSlots : uint8                             // 16 slots
{
	INVENTORY_SLOT_ITEM_START = 23,
	INVENTORY_SLOT_ITEM_END = 39
};

enum BankItemSlots                                          // 28 slots
{
	BANK_SLOT_ITEM_START = 39,
	BANK_SLOT_ITEM_END = 67
};

enum BankBagSlots                                           // 7 slots
{
	BANK_SLOT_BAG_START = 67,
	BANK_SLOT_BAG_END = 74
};

enum BuyBackSlots                                           // 12 slots
{
	BUYBACK_SLOT_START = 74,
	BUYBACK_SLOT_END = 86
};

enum KeyRingSlots : uint8                                   // 32 slots
{
	KEYRING_SLOT_START = 86,
	KEYRING_SLOT_END = 118
};

enum CurrencyTokenSlots                                     // 32 slots
{
	CURRENCYTOKEN_SLOT_START = 118,
	CURRENCYTOKEN_SLOT_END = 150
};