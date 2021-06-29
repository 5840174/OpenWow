#include "stdafx.h"

// General
#include "WoWSettingsGroup.h"

namespace
{
const float cBaseRenderDistance = 500.0f;
}

CWoWSettingsGroup::CWoWSettingsGroup()
	: CPropertiesGroup("WoWSettings", "WoWSettings")
{
	// Distances
	AddProperty(MakeShared(CProperty<float>, "MapChunkRenderDistance", "", cBaseRenderDistance * 2.0f));
	AddProperty(MakeShared(CProperty<float>, "ADT_MCNK_HighRes_Distance", "", cBaseRenderDistance));

	AddProperty(MakeShared(CProperty<float>, "MapTileM2RenderDistance", "", cBaseRenderDistance));
	AddProperty(MakeShared(CProperty<float>, "MapTileWMORenderDistance", "", cBaseRenderDistance));
	AddProperty(MakeShared(CProperty<float>, "MapTileWMODoodadsRenderDisance", "", cBaseRenderDistance / 2.0f));

	AddProperty(MakeShared(CProperty<float>, "M2BaseRenderDistance", "", cBaseRenderDistance * 2.0f));
	AddProperty(MakeShared(CProperty<float>, "WMOBaseRenderDistance", "", cBaseRenderDistance * 2.0f));

	// Drawing objects
	//AddProperty(MakeShared(CProperty<bool>, "draw_mcnk", "", true));
	//AddProperty(MakeShared(CProperty<bool>, "draw_mcnk_low", "", true));
	//AddProperty(MakeShared(CProperty<bool>, "draw_map_wmo", "", true));
	//AddProperty(MakeShared(CProperty<bool>, "draw_wmo_doodads", "", true));
	//AddProperty(MakeShared(CProperty<bool>, "draw_map_m2", "", true));
	//AddProperty(MakeShared(CProperty<bool>, "draw_water", "", true));
	//AddProperty(MakeShared(CProperty<bool>, "draw_map_water", "", true));
	//AddProperty(MakeShared(CProperty<bool>, "draw_wmo_water", "", true));

	//AddProperty(MakeShared(CProperty<bool>, "drawfog", "", true));
}

CWoWSettingsGroup::~CWoWSettingsGroup()
{}
