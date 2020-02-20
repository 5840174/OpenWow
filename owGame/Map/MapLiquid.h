#pragma once

#include "Liquid/Liquid.h"
#include "MapChunk.h"

class CADT_Liquid 
	: public CLiquid
{
public:
	CADT_Liquid(IRenderDevice& RenderDevice, const std::shared_ptr<IByteBuffer>& Bytes, const ADT_MCNK_Header& header);
    virtual ~CADT_Liquid();
};