#include "stdafx.h"

// General
#include "MapLiquid.h"

// FORWARD BEGIN
const DBC_LiquidTypeRecord* getLiquidType(const ADT_MCNK_Header& Header, const CDBCStorage* DBCStorage);
// FORWARD END

CADT_Liquid::CADT_Liquid(IRenderDevice& RenderDevice, const std::shared_ptr<IByteBuffer>& Bytes, const ADT_MCNK_Header& header) :
	CLiquid(RenderDevice, 8, 8)
{
	createLayers(getLiquidType(header, m_RenderDevice.GetBaseManager().GetManager<CDBCStorage>()), Bytes);
}

CADT_Liquid::~CADT_Liquid()
{
}



const DBC_LiquidTypeRecord* getLiquidType(const ADT_MCNK_Header& Header, const CDBCStorage* DBCStorage)
{
	if (Header.flags.lq_river)
	{
		return DBCStorage->DBC_LiquidType()[1];
	}
	else if (Header.flags.lq_ocean)
	{
		return DBCStorage->DBC_LiquidType()[2];
	}
	else if (Header.flags.lq_magma)
	{
		return DBCStorage->DBC_LiquidType()[3];
	}
	else if (Header.flags.lq_slime)
	{
		return DBCStorage->DBC_LiquidType()[4];
	}

	return nullptr;
}
