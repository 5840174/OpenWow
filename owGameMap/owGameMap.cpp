#include "stdafx.h"

// General
#include "owGameMap.h"

// Additional
#include "WMO/WMOsManager.h"
#include "Map.h"
#include "GameState/GameState_Map.h"

extern CLog* gLogInstance;

class COpenWoWMapAndWMOPlguin
	: public IznPlugin
	, public ISceneNodeCreator
	, public ISceneCreator
{
public:
	COpenWoWMapAndWMOPlguin(IBaseManager* BaseManager)
		: m_BaseManager(BaseManager)
	{}
	virtual ~COpenWoWMapAndWMOPlguin()
	{}



	//
	// IznPlugin
	//
	bool Initialize() override
	{
		gLogInstance = dynamic_cast<CLog*>(m_BaseManager->GetManager<ILog>());

		OpenDBs(m_BaseManager);

		std::shared_ptr<IWMOManager> wmoManager = std::make_shared<WMOsManager>(m_BaseManager);
		m_BaseManager->AddManager<IWMOManager>(wmoManager);

		return true;
	}
	void Finalize() override
	{
	
	}
	std::string GetName() const override
	{
		return "Open WoW Map & WMO object plugin.";
	}
	std::string GetDescription() const override
	{
		return "";
	}



	//
	// ISceneNodeCreator
	//
	size_t GetSceneNodesCount() const override
	{
		return 1;
	}
	std::string GetSceneNodeTypeName(size_t Index) const override
	{
		if (Index == 0)
		{
			return "WoWMap";
		}

		return nullptr;
	}
	ISceneNode3D* CreateSceneNode3D(ISceneNode3D* Parent, size_t Index) const override
	{
		if (Index == 0)
		{
			return Parent->CreateSceneNode<CMap>(m_BaseManager);
		}

		return nullptr;
	}



	//
	// ISceneCreator
	//
	size_t GetScenesCount() const override
	{
		return 1;
	}
	std::string GetSceneTypeName(size_t Index) const override
	{
		return "GameState_Map";
	}
	std::shared_ptr<IScene> CreateScene(size_t Index) const override
	{
		return std::make_shared<CGameState_Map>(m_BaseManager);
	}

private:
	IBaseManager* m_BaseManager;
};



IznPlugin* plugin = nullptr;
extern "C" __declspec(dllexport) IznPlugin* WINAPI GetPlugin(IBaseManager* BaseManager)
{
	if (plugin == nullptr)
	{
		plugin = new COpenWoWMapAndWMOPlguin(BaseManager);
	}

	return plugin;
}