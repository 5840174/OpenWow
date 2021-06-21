#pragma once

#include "DBC/DBC__File.h"

DBC_DEF_BEGIN(DBC_LightParams)

__DBC_TVALUE(uint32,			HighlightSky,		2);
__DBC_REF_ID(DBC_LightSkybox,	LightSkyboxID,		3);
__DBC_TVALUE(uint32,            CloudTypeID,        4);
__DBC_TVALUE(float,				Glow,				5); // This controls how much Fog gets added to everything (!) and is used in some places to make them look extra bright
__DBC_TVALUE(float,				WaterShallowAlpha,	6); // Controls how transparent the water is for lakes & rivers.
__DBC_TVALUE(float,				WaterDeepAlpha,		7);
__DBC_TVALUE(float,				OceanShallowAlpha,	8); // Controls how transparent the water is for the ocean.
__DBC_TVALUE(float,				OceanDeepAlpha,		9);

DBC_DEF_END