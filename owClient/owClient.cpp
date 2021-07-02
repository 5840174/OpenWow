#include "stdafx.h"

// General
#include "owClient.h"

extern CLog* gLogInstance;

class ZN_API COpenWoWClientPlguin
	: public IznPlugin
{
public:
	COpenWoWClientPlguin(IBaseManager& BaseManager)
		: m_BaseManager(BaseManager)
	{}
	virtual ~COpenWoWClientPlguin()
	{}



	//
	// IznPlugin
	//
	bool Initialize() override
	{
		gLogInstance = dynamic_cast<CLog*>(m_BaseManager.GetManager<ILog>());
		return true;
	}
	void Finalize() override
	{

	}
	std::string GetName() const override
	{
		return "OpenWoW client plugin.";
	}
	std::string GetDescription() const override
	{
		return "";
	}

private:
	IBaseManager& m_BaseManager;
};



IznPlugin* plugin = nullptr;
extern "C" __declspec(dllexport) IznPlugin* WINAPI GetPlugin(IBaseManager& BaseManager)
{
	if (plugin == nullptr)
		plugin = new COpenWoWClientPlguin(BaseManager);

	return plugin;
}