#pragma once

class CWoWSettingsGroup
	: public CSettingsGroup
{
public:
	CWoWSettingsGroup();
	virtual ~CWoWSettingsGroup();

	// ISettingGroup
	void AddDefaultSettings() override;
};