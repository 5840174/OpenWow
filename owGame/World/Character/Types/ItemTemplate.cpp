#include <stdafx.h>

// General
#include "ItemTemplate.h"

CInet_ItemTemplate::CInet_ItemTemplate(CByteBuffer & b)
{
	b.read(&m_DisplayId);
	b.read(&m_InventoryType);
	b.read(&m_EnchantAuraID);
}


/*ItemTemplate::ItemTemplate(std::shared_ptr<const DBC_ItemDisplayInfoRecord> _display, DBCItem_EInventoryType::List _inventoryType, uint32 _enchantAuraID) :
	DisplayId(0),
	DBCItem_EInventoryType(DBCItem_EInventoryType::NON_EQUIP),
	EnchantAuraID(0)
{
	if (_display == nullptr)
	{
		return;
	}

	DisplayId = _display->Get_ID();
	DBCItem_EInventoryType = _inventoryType;
	EnchantAuraID = _enchantAuraID;
}*/

void CInet_ItemTemplate::TemplateSet(const CInet_ItemTemplate & _o)
{
	m_DisplayId = _o.m_DisplayId;
	m_InventoryType = _o.m_InventoryType;
	m_EnchantAuraID = _o.m_EnchantAuraID;
}

void CInet_ItemTemplate::TemplatePrint()
{
	Log::Print("Item(%d, (DBCItem_EInventoryType::List) %d, %d);", m_DisplayId, m_InventoryType, m_EnchantAuraID);
}