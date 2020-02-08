#include "stdafx.h"

// General
#include "owGame.h"

// Additional
#include "DBC/Core/DBC__Storage.h"
#include "Settings/WoWSettingsGroup.h"
#include "MPQFilesStorage.h"
#include "LiquidInstance.h"

#include "RenderPass_Liquid.h"

extern CLog* gLogInstance;

class ZN_API COpenWoWGamePlguin
	: public IznPlugin
	, public ISceneNodeCreator
	, public IRenderPassCreator
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
		m_BaseManager->GetManager<IFilesManager>()->AddFilesStorage("MPQStorage", std::make_shared<CMPQFilesStorage>("D:\\_games\\World of Warcraft 1.12.1\\Data\\", IFilesStorageEx::Priority::PRIOR_HIGH));

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
	size_t GetSceneNodesCount() const override
	{
		return 1;
	}
	std::string GetSceneNodeTypeName(size_t Index) const override
	{
		if (Index == 0)
		{
			return "WoWLiquid";
		}

		throw std::exception(("COpenWoWGamePlguin::GetSceneNodeTypeName: Index '" + std::to_string(Index) + "' out of bounds.").c_str());
	}
	ISceneNode3D* CreateSceneNode3D(ISceneNode3D* Parent, size_t Index) const override
	{
		if (Index == 0)
		{
			return Parent->CreateSceneNode<Liquid_Instance>();
		}

		throw std::exception(("COpenWoWGamePlguin::CreateSceneNode3D: Index '" + std::to_string(Index) + "' out of bounds.").c_str());
	}
	ISceneNodeUI* CreateSceneNodeUI(ISceneNodeUI* Parent, size_t Index) const override
	{
		throw std::exception(("COpenWoWGamePlguin::CreateSceneNodeUI: Index '" + std::to_string(Index) + "' out of bounds.").c_str());
	}
	


	//
	// IRenderPassCreator
	//
	size_t GetRenderPassCount() const override
	{
		return 1;
	}

	std::string GetRenderPassName(size_t Index) const override
	{
		if (Index == 0)
		{
			return "LiquidPass";
		}

		return "<error>";
	}

	std::shared_ptr<IRenderPass> CreateRenderPass(size_t Index, IRenderDevice& RenderDevice, std::shared_ptr<IRenderTarget> RenderTarget, const Viewport * Viewport, std::shared_ptr<IScene> Scene) const override
	{
		if (Index == 0)
		{
			std::shared_ptr<CRenderPass_Liquid> pass = std::make_shared<CRenderPass_Liquid>(RenderDevice, Scene);
			pass->CreatePipeline(RenderTarget, Viewport);
			return pass;
		}
		
		throw std::exception(("COpenWoWGamePlguin::CreateRenderPass: Index '" + std::to_string(Index) + "' out of bounds.").c_str());
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