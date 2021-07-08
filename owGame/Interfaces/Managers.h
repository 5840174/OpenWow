#pragma once

// FORWARD BEGIN
class CWMO;
class CM2;
// FORWARD END

ZN_INTERFACE ZN_API IWoWObjectsCreator
	: public IManager
{
	virtual ~IWoWObjectsCreator() {};

	ZN_OBJECTCLASS(cWowWorldCreatorManagerCustom)

	virtual void                  ClearCache() = 0;

#ifdef USE_M2_MODELS
	virtual std::shared_ptr<CM2>  LoadM2(IRenderDevice& RenderDevice, const std::string& Filename) = 0;
#endif
	virtual std::shared_ptr<CWMO> LoadWMO(IRenderDevice& RenderDevice, const std::string& Filename) = 0;

	virtual void                         InitEGxBlend(IRenderDevice& RenderDevice) = 0;
	virtual std::shared_ptr<IBlendState> GetEGxBlend(uint32 Index) const = 0;
};


ZN_INTERFACE ZN_API IWoWSceneNode
{
	virtual void UpdateWoW(const UpdateEventArgs& e) = 0;
};