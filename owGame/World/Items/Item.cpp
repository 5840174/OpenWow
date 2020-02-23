#include "stdafx.h"

// General
#include "Item.h"
#include "..\..\Client\Item.h"

Item::Item() 
{}

void Item::Init(CInet_ItemTemplate& _template)
{
	TemplateSet(_template);
}
