#include <stdafx.h>

// General
#include "ItemTemplate.h"

CInet_ItemTemplate::CInet_ItemTemplate() :
	m_DisplayId(0),
	m_InventoryType(EInventoryType::NON_EQUIP),
	m_EnchantAuraID(0)
{}

CInet_ItemTemplate::CInet_ItemTemplate(CByteBuffer & b)
{
	b.readBytes(&m_DisplayId, 4);
	b.readBytes(&m_InventoryType, 1);
	//b.readBytes(&EnchantAuraID, 4);
}

CInet_ItemTemplate::CInet_ItemTemplate(uint32 _displayId, EInventoryType _inventoryType, uint32 _enchantAuraID) :
	m_DisplayId(_displayId),
	m_InventoryType(_inventoryType),
	m_EnchantAuraID(_enchantAuraID)
{}

/*ItemTemplate::ItemTemplate(std::shared_ptr<const DBC_ItemDisplayInfoRecord> _display, EInventoryType::List _inventoryType, uint32 _enchantAuraID) :
	DisplayId(0),
	EInventoryType(EInventoryType::NON_EQUIP),
	EnchantAuraID(0)
{
	if (_display == nullptr)
	{
		return;
	}

	DisplayId = _display->Get_ID();
	EInventoryType = _inventoryType;
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
	Log::Print("Item(%d, (EInventoryType::List) %d, %d);", m_DisplayId, m_InventoryType, m_EnchantAuraID);
}