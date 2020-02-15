#pragma once

#include "Liquid/Liquid.h"
#include "MapChunk.h"

class CADT_Liquid 
	: public CLiquid
{
public:
	CADT_Liquid(IRenderDevice& RenderDevice, uint32 _x, uint32 _y);
    virtual ~CADT_Liquid();

	void CreateFromMCLQ(const std::shared_ptr<IByteBuffer>& Bytes, ADT_MCNK_Header header);
};