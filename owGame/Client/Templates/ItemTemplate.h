#pragma once

#include "DBC/Tables/DBC_Item.h"

struct CInet_ItemTemplate
{
	CInet_ItemTemplate();
	CInet_ItemTemplate(CByteBuffer& b);
	CInet_ItemTemplate(uint32	_displayId, InventoryType::List _inventoryType, uint32 _enchantAuraID);
	
	void TemplateSet(const CInet_ItemTemplate& _o);

	void TemplatePrint();

	uint32					DisplayId;
	InventoryType::List		InventoryType;
	uint32					EnchantAuraID;
};