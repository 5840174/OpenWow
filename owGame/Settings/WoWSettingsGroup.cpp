#include "stdafx.h"

// General
#include "WoWSettingsGroup.h"

CWoWSettingsGroup::CWoWSettingsGroup()
{}

CWoWSettingsGroup::~CWoWSettingsGroup()
{}

void CWoWSettingsGroup::AddDefaultSettings()
{
	// Distances
	AddSetting("ADT_MCNK_Distance", std::make_shared<CSettingBase<float>>(998.0f * 2.0f));
	AddSetting("ADT_MCNK_HighRes_Distance", std::make_shared<CSettingBase<float>>(384.0f * 0.65f * 2.0f));
	AddSetting("ADT_MDX_Distance", std::make_shared<CSettingBase<float>>(384.0f * 2.0f * 2.0f));
	AddSetting("ADT_WMO_Distance", std::make_shared<CSettingBase<float>>(384.0f * 1.5f * 2.0f));
	AddSetting("WMO_MODD_Distance", std::make_shared<CSettingBase<float>>(64.0f * 2.0f));

	// Drawing objects
	AddSetting("draw_mcnk", std::make_shared<CSettingBase<bool>>(true));
	AddSetting("draw_mcnk_low", std::make_shared<CSettingBase<bool>>(true));
	AddSetting("draw_map_wmo", std::make_shared<CSettingBase<bool>>(true));
	AddSetting("draw_wmo_doodads", std::make_shared<CSettingBase<bool>>(true));
	AddSetting("draw_map_m2", std::make_shared<CSettingBase<bool>>(true));
	AddSetting("draw_water", std::make_shared<CSettingBase<bool>>(true));
	AddSetting("draw_map_water", std::make_shared<CSettingBase<bool>>(true));
	AddSetting("draw_wmo_water", std::make_shared<CSettingBase<bool>>(true));

	AddSetting("drawfog", std::make_shared<CSettingBase<bool>>(true));
}
