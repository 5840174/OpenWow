#pragma once

#include "DBC/Tables/DBC_Item.h"

struct CInet_ItemTemplate
{
	CInet_ItemTemplate();
	CInet_ItemTemplate(CByteBuffer& b);
	CInet_ItemTemplate(uint32	_displayId, EInventoryType _inventoryType, uint32 _enchantAuraID);
	
	void TemplateSet(const CInet_ItemTemplate& _o);
	void TemplatePrint();

	uint32					m_DisplayId;
	uint8 		            m_InventoryType;
	uint32					m_EnchantAuraID;
};