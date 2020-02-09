#include "stdafx.h"

// General
#include "owGame.h"

// Additional
#include "DBC/Core/DBC__Storage.h"
#include "Settings/WoWSettingsGroup.h"
#include "Formats/MPQFilesStorage.h"
#include "Formats/ImageBLP.h"
#include "M2/M2_Manager.h"
#include "WMO/WMOsManager.h"


extern CLog* gLogInstance;

class ZN_API COpenWoWGamePlguin
	: public IznPlugin
{
public:
	COpenWoWGamePlguin(IBaseManager* BaseManager)
		: m_BaseManager(BaseManager)
	{}
	virtual ~COpenWoWGamePlguin()
	{}



	//
	// IznPlugin
	//
	bool Initialize() override
	{
		gLogInstance = dynamic_cast<CLog*>(m_BaseManager->GetManager<ILog>());

		m_BaseManager->GetManager<ISettings>()->AddGroup("WoWSettings", std::make_shared<CWoWSettingsGroup>());
		
		// MPQ
		m_BaseManager->GetManager<IFilesManager>()->AddFilesStorage("MPQStorage", std::make_shared<CMPQFilesStorage>("D:\\_games\\World of Warcraft 1.12.1\\Data\\", IFilesStorageEx::Priority::PRIOR_HIGH));

		// BLP
		m_BaseManager->GetManager<IImagesFactory>()->AddImageLoader(std::make_shared<CImageLoaderT<CImageBLP>>());

		// M2
		std::shared_ptr<IM2Manager> m2Manager = std::make_shared<CM2_Manager>(m_BaseManager);
		m_BaseManager->AddManager<IM2Manager>(m2Manager);

		// WMO
		std::shared_ptr<IWMOManager> wmoManager = std::make_shared<WMOsManager>(m_BaseManager);
		m_BaseManager->AddManager<IWMOManager>(wmoManager);

		OpenDBs(m_BaseManager);

		return true;
	}
	void Finalize() override
	{
	
	}
	std::string GetName() const override
	{
		return "Open WoW main plugin.";
	}
	std::string GetDescription() const override
	{
		return "";
	}

private:
	IBaseManager* m_BaseManager;
};



IznPlugin* plugin = nullptr;
extern "C" __declspec(dllexport) IznPlugin* WINAPI GetPlugin(IBaseManager* BaseManager)
{
	if (plugin == nullptr)
	{
		plugin = new COpenWoWGamePlguin(BaseManager);
	}

	return plugin;
}