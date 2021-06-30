#pragma once

struct CInet_ItemTemplate
{
	CInet_ItemTemplate() 
		: m_DisplayId(0)
		, m_InventoryType((uint8)DBCItem_EInventoryType::NON_EQUIP)
		, m_EnchantAuraID(0)
	{}

	CInet_ItemTemplate(CByteBuffer& b);

	CInet_ItemTemplate(uint32 DisplayId, DBCItem_EInventoryType InventoryType, uint32 EnchantAuraID) 
		: m_DisplayId(DisplayId)
		, m_InventoryType((uint8)InventoryType)
		, m_EnchantAuraID(EnchantAuraID)
	{}
	
	void TemplateSet(const CInet_ItemTemplate& _o);
	void TemplatePrint();

	uint32					m_DisplayId;
	uint8 		            m_InventoryType;
	uint32					m_EnchantAuraID;
};