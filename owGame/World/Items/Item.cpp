#include "stdafx.h"

// General
#include "Item.h"
#include "..\..\Client\WoWItem.h"

Item::Item() 
{}

void Item::Init(CInet_ItemTemplate& _template)
{
	TemplateSet(_template);
}
