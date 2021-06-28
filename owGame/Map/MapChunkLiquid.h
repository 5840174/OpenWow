#pragma once

#include "Liquid/Liquid.h"
#include "Map_Headers.h"

class CMapChunkLiquid 
	: public CLiquid
{
public:
	CMapChunkLiquid(IRenderDevice& RenderDevice, const std::shared_ptr<IByteBuffer>& Bytes, const SMapChunk_MCNK& header);
    virtual ~CMapChunkLiquid();

private:
	const DBC_LiquidTypeRecord* GetLiquidType(const SMapChunk_MCNK& Header, const CDBCStorage* DBCStorage);
};