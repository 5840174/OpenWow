#include "stdafx.h"

// General
#include "MapChunkLiquid.h"

CMapChunkLiquid::CMapChunkLiquid(IRenderDevice& RenderDevice, const std::shared_ptr<IByteBuffer>& Bytes, const ADT_MCNK_Header& header) 
	: CLiquid(RenderDevice, 8, 8)
{
	const DBC_LiquidTypeRecord* liquidTypeRecord = GetLiquidType(header, m_RenderDevice.GetBaseManager().GetManager<CDBCStorage>());

	CRange height;
	Bytes->read(&height);
	m_MinHeight = height.min;
	m_MaxHeight = height.max;

	createLayers(liquidTypeRecord, Bytes, false);
}

CMapChunkLiquid::~CMapChunkLiquid()
{
}



//
// Private
//
const DBC_LiquidTypeRecord * CMapChunkLiquid::GetLiquidType(const ADT_MCNK_Header & Header, const CDBCStorage * DBCStorage)
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

	throw CException("Unknown liquid type.");
}
