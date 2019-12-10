#include "stdafx.h"

// General
#include "owGameM2.h"

// Additional
#include "M2_Manager.h"

extern CLog* gLogInstance;

class COpenWoWM2ModelsPlguin
	: public IznPlugin
{
public:
	COpenWoWM2ModelsPlguin(IBaseManager* BaseManager)
		: m_BaseManager(BaseManager)
	{}
	virtual ~COpenWoWM2ModelsPlguin()
	{}



	//
	// IznPlugin
	//
	bool Initialize() override
	{
		gLogInstance = std::dynamic_pointer_cast<CLog>(GetManager<ILog>(m_BaseManager)).get();

		OpenDBs(m_BaseManager);

		std::shared_ptr<IM2Manager> m2Manager = std::make_shared<CM2_Manager>(m_BaseManager);
		AddManager<IM2Manager>(m_BaseManager, m2Manager);

		return true;
	}
	void Finalize() override
	{

	}
	std::string GetName() const override
	{
		return "Open WoW M2 models plugin.";
	}
	std::string GetDescription() const override
	{
		return "";
	}



	//
	// ISceneNodeCreator
	//
	/*size_t GetSceneNodesCount() const
	{
		return 1;
	}
	std::string GetSceneNodeTypeName(size_t Index) const
	{
		if (Index == 0)
		{
			return "WoWLiquid";
		}

		throw std::exception(("COpenWoWM2ModelsPlguin::GetSceneNodeTypeName: Index '" + std::to_string(Index) + "' out of bounds.").c_str());
	}
	std::shared_ptr<ISceneNode> CreateSceneNode(std::weak_ptr<ISceneNode> Parent, size_t Index) const
	{
		if (Index == 0)
		{
			return Parent.lock()->CreateSceneNode<Liquid_Instance>();
		}

		throw std::exception(("COpenWoWM2ModelsPlguin::CreateSceneNode: Index '" + std::to_string(Index) + "' out of bounds.").c_str());
	}*/


private:
	IBaseManager* m_BaseManager;
};



IznPlugin* plugin = nullptr;
extern "C" __declspec(dllexport) IznPlugin* WINAPI GetPlugin(IBaseManager* BaseManager)
{
	if (plugin == nullptr)
	{
		plugin = new COpenWoWM2ModelsPlguin(BaseManager);
	}

	return plugin;
}