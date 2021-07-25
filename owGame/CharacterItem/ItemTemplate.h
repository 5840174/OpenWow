#pragma once

struct SCharacterItemTemplate
{
	SCharacterItemTemplate() 
		: DisplayId(0)
		, InventoryType((uint8)DBCItem_EInventoryItemType::NON_EQUIP)
		, EnchantAuraID(0)
	{}

	SCharacterItemTemplate(CByteBuffer& b)
	{
		b >> DisplayId;
		b >> InventoryType;
		b >> EnchantAuraID;
	}

	SCharacterItemTemplate(uint32 DisplayId, DBCItem_EInventoryItemType InventoryType, uint32 EnchantAuraID) 
		: DisplayId(DisplayId)
		, InventoryType((uint8)InventoryType)
		, EnchantAuraID(EnchantAuraID)
	{}
	
	void TemplatePrint()
	{
		Log::Print("Item(%d, (DBCItem_EInventoryItemType::List) %d, %d);", DisplayId, InventoryType, EnchantAuraID);
	}

	uint32					DisplayId;
	uint8 		            InventoryType;
	uint32					EnchantAuraID;
};