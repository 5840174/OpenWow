#pragma once

#include "Liquid/Liquid.h"

struct MH2O_Header
{
	uint32 offsetInstances; // This is an Offset to the first Water Layer, it has tobe an offset because there can be multiple layers.
	uint32 layersCount;    // 0 if the chunk has no liquids. If > 1, the offsets will point to arrays.

						   /*
						   This field requires some explanation.
						   Its an offset to a uint8[8] data block.
						   the data block contains a bit field 8x8 = 64 bit every bit represents a quad between 4 verts.
						   But the bits in this field are only set to 1 if the field is really visible this is not ment in a
						   technical way. To make it clear all of the 4 vertices have an alpha value (i explain them later)
						   if only one of these alpha values smaller then 10 the bit is not set

						   if the first byte is 0x17h which is in binary 00010111b it has to look like this in a grid:

						   0123...
						   1+-----> bits
						   2| 11101000
						   3|
						   .|
						   .V
						   bytes

						   */
	uint32 offsetAttributes;
};

class CMapTileLiquid
	: public CLiquid
{
public:
	CMapTileLiquid(IRenderDevice& RenderDevice, const std::shared_ptr<IByteBuffer>& Bytes, const MH2O_Header& header);
	virtual ~CMapTileLiquid();
};