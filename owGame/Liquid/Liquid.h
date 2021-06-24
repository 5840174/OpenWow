#pragma once

#include "LiquidModel.h"

#pragma pack(push,1)

struct SLiquidVertex
{
	union
	{
		struct SWaterVert
		{
			int8 depth;
			int8 flow0Pct;
			int8 flow1Pct;
			int8 unk0;
			float height;
		} waterVert;

		struct SOceanVert
		{
			uint8 depth;
			uint8 foam;
			uint8 wet;
			uint8 unk0;
		} oceanVert;

		struct SMagmaVert
		{
			uint16 s;
			uint16 t;
			float height;
		} magmaVert;
	};
};

struct SLiquidFlag
{
	uint8 liquid : 6;
	// 0x01
	// 0x02
	// 0x04
	// 0x08 // no render
	// 0x10
	// 0x20

	uint8 fishable : 1;  // 0x40
	uint8 shared : 1;    // 0x80
};

#pragma pack(pop)



//
// CLiquid
//
class ZN_API CLiquid
	: public ISceneNodeProvider
{
public:
	CLiquid(IRenderDevice& RenderDevice, uint32 x, uint32 y);
    virtual ~CLiquid();

	// ISceneNodeProvider
	void CreateInsances(const std::shared_ptr<ISceneNode>& Parent) const override;

	float getMinHeight() const;
	float getMaxHeight() const;

protected:
	void createLayers(const DBC_LiquidTypeRecord* LiquidTypeRecord, const std::shared_ptr<IByteBuffer>& Bytes, bool NeedInvertY);
	
protected:
	float m_MinHeight;
	float m_MaxHeight;

public:
	uint32 m_TilesX;
	uint32 m_TilesY;

	std::vector<std::shared_ptr<CLiquidModel>> m_WaterLayers;

protected:
	IRenderDevice& m_RenderDevice;
};
