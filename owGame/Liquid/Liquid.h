#pragma once

#include "LiquidLayer.h"

class ZN_API CLiquid
{
public:
	CLiquid(IRenderDevice& RenderDevice, uint32 x, uint32 y);
    virtual ~CLiquid();

protected:
	void createLayers(std::shared_ptr<const DBC_LiquidTypeRecord> _type, const std::shared_ptr<IByteBuffer>& Bytes);
	
public:
	uint32                                                              m_TilesX, m_TilesY;
	BoundingBox                                                         m_Bounds;

	std::vector<std::shared_ptr<CLiquidLayer>>							m_WaterLayers;

	float                                                               ydir;

protected:
	IRenderDevice& m_RenderDevice;

private:
	const vec3 defaultNormal = vec3(0.0f, 1.0f, 0.0f);
};
