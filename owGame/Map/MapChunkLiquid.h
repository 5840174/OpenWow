#pragma once

#include "Liquid/Liquid.h"
#include "Map_Headers.h"

class CMapChunkLiquid 
	: public CLiquid
{
public:
	CMapChunkLiquid(IRenderDevice& RenderDevice, const std::shared_ptr<IByteBuffer>& Bytes, const ADT_MCNK_Header& header);
    virtual ~CMapChunkLiquid();

private:
	const DBC_LiquidTypeRecord* GetLiquidType(const ADT_MCNK_Header& Header, const CDBCStorage* DBCStorage);
};