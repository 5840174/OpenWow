#pragma once

#include "LiquidLayer.h"

#pragma region Types
#include __PACK_BEGIN

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
			uint8_t depth;
			uint8_t foam;
			uint8_t wet;
			uint8_t unk0;
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
	uint8 liquid : 6;    // 0x01 - 0x20
	uint8 fishable : 1;  // 0x40
	uint8 shared : 1;    // 0x80
};

#include __PACK_END
#pragma endregion

class ZN_API CLiquid
	: public ISceneNodeProvider
{
public:
	CLiquid(IRenderDevice& RenderDevice, uint32 x, uint32 y);
    virtual ~CLiquid();

	// ISceneNodeProvider
	void CreateInsances(const std::shared_ptr<ISceneNode3D>& Parent) const override;

protected:
	void createLayers(const DBC_LiquidTypeRecord* _type, const std::shared_ptr<IByteBuffer>& Bytes);
	
public:
	uint32                                                              m_TilesX, m_TilesY;
	std::vector<std::shared_ptr<CLiquidLayer>>							m_WaterLayers;

	float                                                               ydir;

protected:
	IRenderDevice& m_RenderDevice;

private:
	const glm::vec3 defaultNormal = glm::vec3(0.0f, 1.0f, 0.0f);
};
