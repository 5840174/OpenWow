#pragma once

struct CInet_ItemTemplate
{
	CInet_ItemTemplate() 
		: ItemDisplayId(0)
		, InventoryType((uint8)DBCItem_EInventoryItemSlot::NON_EQUIP)
		, EnchantAuraID(0)
	{}

	CInet_ItemTemplate(CByteBuffer& b)
	{
		b >> ItemDisplayId;
		b >> InventoryType;
		b >> EnchantAuraID;
	}

	CInet_ItemTemplate(uint32 DisplayId, DBCItem_EInventoryItemSlot InventoryType, uint32 EnchantAuraID) 
		: ItemDisplayId(DisplayId)
		, InventoryType((uint8)InventoryType)
		, EnchantAuraID(EnchantAuraID)
	{}
	
	void TemplatePrint()
	{
		Log::Print("Item(%d, (DBCItem_EInventoryItemSlot::List) %d, %d);", ItemDisplayId, InventoryType, EnchantAuraID);
	}

	uint32					ItemDisplayId;
	uint8 		            InventoryType;
	uint32					EnchantAuraID;
};