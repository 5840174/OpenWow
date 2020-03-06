#pragma once

// FORWARD BEGIN
class CWMO;
class CM2;
// FORWARD END

ZN_INTERFACE ZN_API __declspec(uuid("42D47100-B825-47F1-BE2F-6F7C78443884")) IWoWObjectsCreator
	: public IManager
{
	virtual ~IWoWObjectsCreator() {};

	virtual void                  ClearCache() = 0;

	virtual std::shared_ptr<CM2>   LoadM2(IRenderDevice& RenderDevice, const std::string& Filename) = 0;
	virtual std::shared_ptr<CWMO> LoadWMO(IRenderDevice& RenderDevice, const std::string& Filename) = 0;
};
