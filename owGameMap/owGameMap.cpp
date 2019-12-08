#include "stdafx.h"

// General
#include "owGameMap.h"


extern CLog* gLogInstance;

class COpenWoWMapAndWMOPlguin
	: public IznPlugin
	, public ISceneNodeCreator
{
public:
	COpenWoWMapAndWMOPlguin(IBaseManager* BaseManager)
		: m_BaseManager(BaseManager)
	{
		OpenDBs(m_BaseManager);
	}
	virtual ~COpenWoWMapAndWMOPlguin()
	{}



	//
	// IznPlugin
	//
	bool Initialize() override
	{
		gLogInstance = std::dynamic_pointer_cast<CLog>(GetManager<ILog>(m_BaseManager)).get();

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
	std::shared_ptr<ISceneNode> CreateSceneNode(std::shared_ptr<ISceneNode> Parent, size_t Index) const
	{
		if (Index == 0)
		{
			return nullptr;//Parent->CreateSceneNode<Liquid_Instance>();
		}

		throw std::exception(("COpenWoWGamePlguin::CreateSceneNode: Index '" + std::to_string(Index) + "' out of bounds.").c_str());
	}


private:
	IBaseManager* m_BaseManager;
	std::shared_ptr<IRenderDevice> m_CachedRenderDevice;
};



IznPlugin* plugin = nullptr;
IznPlugin* WINAPI GetPlugin(IBaseManager* BaseManager)
{
	if (plugin == nullptr)
	{
		plugin = new COpenWoWMapAndWMOPlguin(BaseManager);
	}

	return plugin;
}