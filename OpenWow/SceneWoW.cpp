#include "stdafx.h"

// Gerenal
#include "SceneWoW.h"

// Additional
#include "Client/Client.h"

CSceneWoW::CSceneWoW(IBaseManager& BaseManager)
	: SceneBase(BaseManager)
{}

CSceneWoW::~CSceneWoW()
{
	CMapWMOInstance::reset();
#ifdef USE_M2_MODELS
	CMapM2Instance::reset();
#endif

	OutputDebugStringW(L"Destroyed.");
}


//
// IGameState
//
void CSceneWoW::Initialize()
{
	SceneBase::Initialize();

	auto cameraNode = GetRootNode3D()->CreateSceneNode<SceneNode3D>();
	cameraNode->AddComponent(std::make_shared<CCameraComponent3D>(*cameraNode));

	SetCameraController(std::make_shared<CFreeCameraController>());
	GetCameraController()->SetCamera(cameraNode->GetComponent<ICameraComponent3D>());
	GetCameraController()->GetCamera()->SetPerspectiveProjection(ICameraComponent3D::EPerspectiveProjectionHand::Right, 45.0f, static_cast<float>(GetRenderWindow()->GetWindowWidth()) / static_cast<float>(GetRenderWindow()->GetWindowHeight()), 0.5f, 10000.0f);

	
	Load3D();
	//Load3D_M2s();
	//TestCreateMap(MapID);
	//LoadUI();


	std::shared_ptr<CSceneCreateTypedListsPass> sceneListPass = std::make_shared<CSceneCreateTypedListsPass>(GetRenderDevice(), shared_from_this());

	m_Technique3D.AddPass(GetBaseManager().GetManager<IRenderPassFactory>()->CreateRenderPass("ClearPass", GetRenderDevice(), GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport(), shared_from_this()));
	//m_Technique3D.AddPass(wmoListPass);
	//m_Technique3D.AddPass(m2ListPass);
	m_Technique3D.AddPass(sceneListPass);
	m_Technique3D.AddPass(std::make_shared<CRenderPass_Sky>(GetRenderDevice(), sceneListPass)->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	//m_Technique3D.AddPass(std::make_shared<CRenderPass_WDL>(GetRenderDevice(), shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	m_Technique3D.AddPass(std::make_shared<CRenderPass_ADT_MCNK>(GetRenderDevice(), sceneListPass)->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	
	m_Technique3D.AddPass(std::make_shared<CRenderPass_WMO>(GetRenderDevice(), sceneListPass)->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	
	m_Technique3D.AddPass(std::make_shared<CRenderPass_Liquid>(GetRenderDevice(), sceneListPass)->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));

	//m_Technique3D.AddPass(std::make_shared<CRenderPass_WMO2>(GetRenderDevice(), wmoListPass, shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	
	//m_Technique3D.AddPass(std::make_shared<CRenderPass_M2>(GetRenderDevice(), sceneListPass, true)->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	//m_Technique3D.AddPass(std::make_shared<CRenderPass_M2>(GetRenderDevice(), sceneListPass, false)->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	
	m_Technique3D.AddPass(std::make_shared<CRenderPass_M2_Instanced>(GetRenderDevice(), sceneListPass, true)->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	m_Technique3D.AddPass(std::make_shared<CRenderPass_M2_Instanced>(GetRenderDevice(), sceneListPass, false)->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	
	//m_Technique3D.AddPass(std::make_shared<CDrawBoundingBoxPass>(GetRenderDevice(), shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	m_Technique3D.AddPass(std::make_shared<CMaterialParticlePass>(GetRenderDevice(), shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
}

void CSceneWoW::Finalize()
{
	// Insert code here

	SceneBase::Finalize();
}


void CSceneWoW::OnRayIntersected(const glm::vec3& Point)
{

}



//
//
//
void CSceneWoW::OnPreRender(RenderEventArgs& e)
{
	CMapWMOInstance::reset();
#ifdef USE_M2_MODELS
	CMapM2Instance::reset();
#endif

	//m2Instance->SetRotation(glm::vec3(m2Instance->GetRotation().x, m2Instance->GetRotation().y + 0.05f * e.DeltaTime / 60.0f, 0.0f));

	SceneBase::OnPreRender(e);
}



//
// Keyboard events
//
bool CSceneWoW::OnWindowKeyPressed(KeyEventArgs & e)
{
	if (e.Key == KeyCode::C)
	{
		TestCreateMap(530);
		return true;
	}
	else if (e.Key == KeyCode::V)
	{
		TestDeleteMap();
		return true;
	}

	return SceneBase::OnWindowKeyPressed(e);
}

void CSceneWoW::OnWindowKeyReleased(KeyEventArgs & e)
{
	SceneBase::OnWindowKeyReleased(e);
}



//
//
//
void CSceneWoW::Load3D()
{
	//auto wmo = GetBaseManager().GetManager<IWoWObjectsCreator>()->LoadWMO(GetRenderDevice(), /*"World\\wmo\\Lorderon\\Undercity\\Undercity.wmo"*/"World\\wmo\\Outland\\DarkPortal\\DarkPortal_Temple.wmo");
	//wmoInstance = GetRootNode3D()->CreateSceneNode<CWMO_Base_Instance>(wmo);
	//GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(wmoInstance);

	//auto m2 = GetBaseManager().GetManager<IWoWObjectsCreator>()->LoadM2(GetRenderDevice(), "World\\Expansion01\\Doodads\\Netherstorm\\BioDomes\\NS_BioDome_All_FX_South.M2");
	//m2Instance = GetRootNode3D()->CreateSceneNode<CM2_Base_Instance>(m2);
	//GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(m2Instance);

	auto m2 = GetBaseManager().GetManager<IWoWObjectsCreator>()->LoadM2(GetRenderDevice(), "World\\Expansion01\\Doodads\\Netherstorm\\Float_particles\\Netherstorm_Particles_Pink.mdx");
	m2Instance = GetRootNode3D()->CreateSceneNode<CM2_Base_Instance>(m2);
	//m2Instance->SetScale(glm::vec3(100.0f));
	GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(m2Instance);

	

	skyManager = GetRootNode3D()->CreateSceneNode<SkyManager>(GetRenderDevice());
	GetBaseManager().AddManager<ISkyManager>(skyManager);
	skyManager->Load(0);

	map = GetRootNode3D()->CreateSceneNode<CMap>(GetBaseManager(), GetRenderDevice());

#if 0
	auto wmo = GetBaseManager().GetManager<IWMOManager>()->Add(GetRenderDevice(), "World\\wmo\\Azeroth\\Buildings\\Stormwind\\Stormwind.wmo");
	wmoInstance = GetRootNode3D()->CreateSceneNode<CWMO_Base_Instance>(*wmo);
	wmoInstance->SetTranslate(glm::vec3(100, 500, 100));
	wmoInstance->SetRotation(glm::vec3(0.0f, glm::pi<float>(), 0.0f));
	GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(wmoInstance.get());
#endif

	GetCameraController()->GetCamera()->SetTranslation(glm::vec3());
	GetCameraController()->GetCamera()->SetYaw(48.8);
	GetCameraController()->GetCamera()->SetPitch(-27.8);
}

void CSceneWoW::Load3D_M2s()
{
	Random r(time(0));

	CWorldObjectCreator creator(GetBaseManager());

	const auto& records = GetBaseManager().GetManager<CDBCStorage>()->DBC_CreatureDisplayInfo().Records();
	for (size_t i = 0; i < 25; i++)
	{
		for (size_t j = 0; j < 25; j++)
		{
			size_t id = r.NextUInt() % records.size();

			while (true)
			{
				const auto& data = records.find(id);
				if (data != records.end())
				{
					break;
				}

				id = r.NextUInt() % records.size();
			}

			auto creature = creator.BuildCreatureFromDisplayInfo(GetRenderDevice(), this, id, GetRootNode3D());
			if (creature != nullptr)
			{
				creature->SetTranslate(glm::vec3(i * 17.5f, 0.0f, j * 17.5f));
				if (creature->getAnimator())
					creature->getAnimator()->PlayAnimation(4, true);
			}
		}
	}

	GetCameraController()->GetCamera()->SetTranslation(glm::vec3(150, 150, 150));
	GetCameraController()->GetCamera()->SetYaw(235);
	GetCameraController()->GetCamera()->SetPitch(-45);
}


void CSceneWoW::LoadUI()
{
#if 1
	rootForBtns = GetRootNodeUI()->CreateSceneNode<SceneNodeUI>();

	minimap = GetRootNodeUI()->CreateSceneNode<CUITextureNode>(GetRenderDevice(), glm::vec2(256, 256));
	minimap->SetTranslate(glm::vec2(900, 800));
	minimap->SetOnClickCallback([this](const ISceneNodeUI* Node, glm::vec2 Point) { this->GoToCoord(Node, Point); });

	size_t cntrX = 0;
	size_t cntrY = 0;

	auto dbcMap = GetBaseManager().GetManager<CDBCStorage>();
	for (const auto& it : dbcMap->DBC_Map())
	{
		auto btn = rootForBtns->CreateSceneNode<CUIButtonNode>(GetRenderDevice());
		btn->CreateDefault();
		volatile std::wstring name = it->Get_Name();

		btn->SetText(it->Get_Directory());

		uint32 id = it->Get_ID();
		btn->SetOnClickCallback([this, id] (const ISceneNodeUI* Node, glm::vec2 Point) { this->TestCreateMap(id); });
		btn->SetTranslate(glm::vec2(cntrX * 180, cntrY * 38));

		cntrY++;
		if (cntrY > 20)
		{
			cntrY = 0;
			cntrX++;
		}
	}

#endif
	m_TechniqueUI.AddPass(std::make_shared<CUIButtonPass>(GetRenderDevice(), shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	m_TechniqueUI.AddPass(std::make_shared<CUIFontPass>(GetRenderDevice(), shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	m_TechniqueUI.AddPass(std::make_shared<CUITexturePass>(GetRenderDevice(), shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
}

void CSceneWoW::TestCreateMap(uint32 MapID)
{
	//const int32 x = 40;
	//const int32 y = 30;
	//const uint32 mapID = 0;
	//const int32 x = 19; //FOR BC
	//const int32 y = 32;
	const int32 x = 26; //FOR BC 2
	const int32 y = 30;

	skyManager->Load(MapID);

	rootForBtns->GetParent().lock()->RemoveChild(rootForBtns);
	map->Unload();


	map->MapPreLoad(GetBaseManager().GetManager<CDBCStorage>()->DBC_Map()[MapID]);
	map->MapLoad();
	map->EnterMap(x, y);

	minimap->SetTexture(map->getMinimap());

	//GetCameraController()->GetCamera()->SetTranslation(glm::vec3(x * C_TileSize + C_TileSize / 2.0f, 100.0f, y * C_TileSize + C_TileSize / 2.0f));
	//GetCameraController()->GetCamera()->SetYaw(48.8);
	//GetCameraController()->GetCamera()->SetPitch(-27.8);

	//GetCameraController()->GetCamera()->SetTranslation(glm::vec3(0, 100.0f, 0));

	if (map->getGlobalInstance())
	{
		GetCameraController()->GetCamera()->SetTranslation(map->getGlobalInstance()->GetTranslation());
	}
}

void CSceneWoW::GoToCoord(const ISceneNodeUI* Node, const glm::vec2& Point)
{
	Log::Green("Coord %f %f", Point.x, Point.y);
	glm::vec2 conv = (Point / Node->GetSize() * 512.0f) * (C_TileSize * 64.0f / 512.0f);

	GetCameraController()->GetCamera()->SetTranslation(glm::vec3(conv.x, GetCameraController()->GetCamera()->GetTranslation().y, conv.y));
}

void CSceneWoW::TestDeleteMap()
{
	map->Unload();
}

