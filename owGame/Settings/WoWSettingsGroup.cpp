#include "stdafx.h"

// General
#include "WoWSettingsGroup.h"

CWoWSettingsGroup::CWoWSettingsGroup()
	: CPropertiesGroup("WoWSettings", "WoWSettings")
{
	// Distances
	AddProperty(MakeShared(CProperty<float>, "MapChunkRenderDistance", "", 998.0f * 2.0f));
	AddProperty(MakeShared(CProperty<float>, "ADT_MCNK_HighRes_Distance", "", 384.0f * 0.65f * 2.0f));
	AddProperty(MakeShared(CProperty<float>, "ADT_MDX_Distance", "", 384.0f * 2.0f * 2.0f));
	AddProperty(MakeShared(CProperty<float>, "ADT_WMO_Distance", "", 384.0f * 1.5f * 2.0f));
	AddProperty(MakeShared(CProperty<float>, "WMO_MODD_Distance", "", 64.0f * 2.0f));

	// Drawing objects
	AddProperty(MakeShared(CProperty<bool>, "draw_mcnk", "", true));
	AddProperty(MakeShared(CProperty<bool>, "draw_mcnk_low", "", true));
	AddProperty(MakeShared(CProperty<bool>, "draw_map_wmo", "", true));
	AddProperty(MakeShared(CProperty<bool>, "draw_wmo_doodads", "", true));
	AddProperty(MakeShared(CProperty<bool>, "draw_map_m2", "", true));
	AddProperty(MakeShared(CProperty<bool>, "draw_water", "", true));
	AddProperty(MakeShared(CProperty<bool>, "draw_map_water", "", true));
	AddProperty(MakeShared(CProperty<bool>, "draw_wmo_water", "", true));

	AddProperty(MakeShared(CProperty<bool>, "drawfog", "", true));
}

CWoWSettingsGroup::~CWoWSettingsGroup()
{}
