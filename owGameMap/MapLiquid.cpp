#include "stdafx.h"

// General
#include "MapLiquid.h"

CADT_Liquid::CADT_Liquid(IBaseManager* BaseManager, uint32 _x, uint32 _y) :
	CLiquid(BaseManager, _x, _y)
{}

CADT_Liquid::~CADT_Liquid()
{
}

void CADT_Liquid::CreateFromMCLQ(std::shared_ptr<IFile> f, ADT_MCNK_Header header)
{
	createLayers(header.getLiquidType(), f);
}
