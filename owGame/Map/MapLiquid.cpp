#include "stdafx.h"

// General
#include "MapLiquid.h"

CADT_Liquid::CADT_Liquid(IRenderDevice& RenderDevice, uint32 _x, uint32 _y) :
	CLiquid(RenderDevice, _x, _y)
{}

CADT_Liquid::~CADT_Liquid()
{
}

void CADT_Liquid::CreateFromMCLQ(const std::shared_ptr<IByteBuffer>& Bytes, ADT_MCNK_Header header)
{
	createLayers(header.getLiquidType(), Bytes);
}
