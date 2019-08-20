#pragma once


struct InventoryType
{
    enum List : uint8
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

        COUNT
    };

    static bool isWeapon(InventoryType::List _item)
    {
        if (_item == WEAPON)
            return true;

        if (_item == TWOHWEAPON)
            return true;

        if (_item == WEAPONMAINHAND)
            return true;

        if (_item == WEAPONOFFHAND)
            return true;

        return false;
    }
};

struct CInet_ItemTemplate
{
	CInet_ItemTemplate();
	CInet_ItemTemplate(uint32	_displayId, InventoryType::List _inventoryType, uint32 _enchantAuraID);
	
	void TemplateSet(const CInet_ItemTemplate& _o);
	void TemplateFill(CByteBuffer& b);

	void TemplatePrint();

	uint32					DisplayId;
	InventoryType::List		InventoryType;
	uint32					EnchantAuraID;
};