#include "stdafx.h"

// General
#include "owGame.h"

// Additional
#include "Settings/WoWSettingsGroup.h"
#include "Formats/MPQFilesStorage.h"
#include "Formats/ImageBLP.h"
#include "World/WorldObjectsCreator.h"


IznPlugin* gPlugin = nullptr;
extern CLog* gLogInstance;


class ZN_API COpenWoWGamePlguin
	: public IznPlugin
{
public:
	COpenWoWGamePlguin(IBaseManager& BaseManager)
		: m_BaseManager(BaseManager)
	{}
	virtual ~COpenWoWGamePlguin()
	{}



	//
	// IznPlugin
	//
	bool Initialize() override
	{
		gLogInstance = dynamic_cast<CLog*>(m_BaseManager.GetManager<ILog>());

		m_BaseManager.GetManager<ISettings>()->AddGroup(std::make_shared<CWoWSettingsGroup>());
		
		// MPQ
#if WOW_CLIENT_VERSION == WOW_CLASSIC_1_12_1
		m_BaseManager.GetManager<IFilesManager>()->AddFilesStorage("MPQStorage", std::make_shared<CMPQFilesStorage>("D:\\_games\\World of Warcraft 1.12.1\\Data\\", IFilesStorageEx::Priority::PRIOR_HIGH));
#elif WOW_CLIENT_VERSION == WOW_BC_2_4_3
		m_BaseManager.GetManager<IFilesManager>()->AddFilesStorage("MPQStorage", std::make_shared<CMPQFilesStorage>("c:\\_engine\\World of Warcraft 2.4.3\\Data\\", IFilesStorageEx::Priority::PRIOR_HIGH));
#elif WOW_CLIENT_VERSION == WOW_WOTLK_3_3_5
		m_BaseManager.GetManager<IFilesManager>()->AddStorage(EFilesStorageType::GAMEDATA, MakeShared(CMPQFilesStorage, "C:\\Games\\World of Warcraft 3.3.5a\\Data\\"));
#endif
		m_BaseManager.GetManager<IFilesManager>()->AddStorage(EFilesStorageType::GAMEDATA, MakeShared(CLocalFilesStorage, "D:\\OpenWow\\_gamedata\\"));

		// BLP
		m_BaseManager.GetManager<IImagesFactory>()->AddImageLoader(MakeShared(CImageLoaderT<CImageBLP>));

		m_BaseManager.AddManager<CDBCStorage>(MakeShared(CDBCStorage, m_BaseManager));
		m_BaseManager.AddManager<IWoWObjectsCreator>(MakeShared(CWorldObjectCreator, m_BaseManager));


		return true;
	}
	void Finalize() override
	{
	
	}
	std::string GetName() const override
	{
		return "OpenWoW game plugin";
	}
	std::string GetDescription() const override
	{
		return "";
	}

private:
	IBaseManager& m_BaseManager;
};




IznPlugin* WINAPI GetPlugin(IBaseManager& BaseManager)
{
	if (gPlugin == nullptr)
		gPlugin = new COpenWoWGamePlguin(BaseManager);
	return gPlugin;
}