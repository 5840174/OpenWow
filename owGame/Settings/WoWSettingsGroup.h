#pragma once

class CWoWSettingsGroup
	: public CSettingsGroupBase
{
public:
	CWoWSettingsGroup();
	virtual ~CWoWSettingsGroup();

	// ISettingGroup
	void AddDefaultSettings() override;
};