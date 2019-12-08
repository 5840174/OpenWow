#include "stdafx.h"

// General
#include "owGame.h"

// Additional
#include "DBC/Core/DBC__Storage.h"
#include "Settings/WoWSettingsGroup.h"
#include "MPQFilesStorage.h"
#include "LiquidInstance.h"

extern CLog* gLogInstance;

class COpenWoWGamePlguin
	: public IznPlugin
	, public ISceneNodeCreator
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
		gLogInstance = std::dynamic_pointer_cast<CLog>(GetManager<ILog>(m_BaseManager)).get();

		GetManager<ISettings>(m_BaseManager)->AddGroup("WoWSettings", std::make_shared<CWoWSettingsGroup>());
		GetManager<IFilesManager>(m_BaseManager)->RegisterFilesStorage(std::make_shared<CMPQFilesStorage>("D:\\_games\\World of Warcraft 1.12.1\\Data\\", IFilesStorageEx::PRIOR_HIGH));

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



	//
	// ISceneNodeCreator
	//
	size_t GetSceneNodesCount() const
	{
		return 1;
	}
	std::string GetSceneNodeTypeName(size_t Index) const
	{
		if (Index == 0)
		{
			return "WoWLiquid";
		}

		throw std::exception(("COpenWoWGamePlguin::GetSceneNodeTypeName: Index '" + std::to_string(Index) + "' out of bounds.").c_str());
	}
	std::shared_ptr<ISceneNode> CreateSceneNode(std::weak_ptr<ISceneNode> Parent, size_t Index) const
	{
		if (Index == 0)
		{
			return Parent.lock()->CreateSceneNode<Liquid_Instance>();
		}

		throw std::exception(("COpenWoWGamePlguin::CreateSceneNode: Index '" + std::to_string(Index) + "' out of bounds.").c_str());
	}


private:
	IBaseManager* m_BaseManager;
	std::shared_ptr<IRenderDevice> m_CachedRenderDevice;
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