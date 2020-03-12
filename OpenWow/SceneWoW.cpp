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
	CMapM2Instance::reset();

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
	GetCameraController()->GetCamera()->SetPerspectiveProjection(ICameraComponent3D::EPerspectiveProjectionHand::Right, 45.0f, 1.0f/*GetRenderWindow()->GetWindowWidth() / GetRenderWindow()->GetWindowHeight()*/, 0.5f, 10000.0f);

	
	Load3D();
	//Load3D_M2s();
	TestCreateMap();
	LoadUI();


	std::shared_ptr<CSceneCreateTypedListsPass> sceneListPass = std::make_shared<CSceneCreateTypedListsPass>(GetRenderDevice(), shared_from_this());

	m_Technique3D.AddPass(GetBaseManager().GetManager<IRenderPassFactory>()->CreateRenderPass("ClearPass", GetRenderDevice(), GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport(), shared_from_this()));
	//m_Technique3D.AddPass(wmoListPass);
	//m_Technique3D.AddPass(m2ListPass);
	m_Technique3D.AddPass(sceneListPass);
	m_Technique3D.AddPass(std::make_shared<CRenderPass_Sky>(GetRenderDevice(), sceneListPass)->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	//m_Technique3D.AddPass(std::make_shared<CRenderPass_WDL>(GetRenderDevice(), shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	m_Technique3D.AddPass(std::make_shared<CRenderPass_ADT_MCNK>(GetRenderDevice(), sceneListPass)->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	//m_Technique3D.AddPass(std::make_shared<CRenderPass_Liquid>(GetRenderDevice(), sceneListPass)->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	m_Technique3D.AddPass(std::make_shared<CRenderPass_WMO>(GetRenderDevice(), sceneListPass)->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	//m_Technique3D.AddPass(std::make_shared<CRenderPass_WMO2>(GetRenderDevice(), wmoListPass, shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	m_Technique3D.AddPass(std::make_shared<CRenderPass_M2>(GetRenderDevice(), sceneListPass, false)->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	//m_Technique3D.AddPass(std::make_shared<CRenderPass_M2_Instanced>(GetRenderDevice(), sceneListPass, true)->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
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

	SceneBase::OnPreRender(e);
}



//
// Keyboard events
//
bool CSceneWoW::OnWindowKeyPressed(KeyEventArgs & e)
{
	if (e.Key == KeyCode::C)
	{
		TestCreateMap();
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
	auto wmo = GetBaseManager().GetManager<IWoWObjectsCreator>()->LoadWMO(GetRenderDevice(), "World\\wmo\\Outland\\DarkPortal\\DarkPortal_Temple.wmo");
	wmoInstance = GetRootNode3D()->CreateSceneNode<CWMO_Base_Instance>(wmo);
	GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(wmoInstance);

	//auto m2 = GetBaseManager().GetManager<IWoWObjectsCreator>()->LoadM2(GetRenderDevice(), "CREATURE\\Illidan\\IllidanDark.M2");
	//m2Instance = GetRootNode3D()->CreateSceneNode<CM2_Base_Instance>(m2);
	//GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(m2Instance);

	skyManager = GetRootNode3D()->CreateSceneNode<SkyManager>(GetRenderDevice());
	skyManager->Load(0);

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
	for (size_t i = 0; i < 15; i++)
	{
		for (size_t j = 0; j < 15; j++)
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
				//creature->getAnimator()->PlayAnimation(16, true);
			}
		}
	}

	GetCameraController()->GetCamera()->SetTranslation(vec3(150, 150, 150));
	GetCameraController()->GetCamera()->SetYaw(235);
	GetCameraController()->GetCamera()->SetPitch(-45);
}


void CSceneWoW::LoadUI()
{
	m_TechniqueUI.AddPass(std::make_shared<CUIFontPass>(GetRenderDevice(), shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
}

void CSceneWoW::TestCreateMap()
{
	//const int32 x = 40;
	//const int32 y = 30;
	//const uint32 mapID = 0;
	//const int32 x = 19; //FOR BC
	//const int32 y = 32;
	const int32 x = 26; //FOR BC 2
	const int32 y = 30;
	const uint32 mapID = 530;

	if (map != nullptr)
		TestDeleteMap();

	skyManager->Load(mapID);

	map = GetRootNode3D()->CreateSceneNode<CMap>(GetBaseManager(), GetRenderDevice());
	map->MapPreLoad(GetBaseManager().GetManager<CDBCStorage>()->DBC_Map()[mapID]);
	map->MapLoad();
	map->MapPostLoad();
	map->EnterMap(x, y);

	GetCameraController()->GetCamera()->SetTranslation(glm::vec3(x * C_TileSize + C_TileSize / 2.0f, 100.0f, y * C_TileSize + C_TileSize / 2.0f));
	GetCameraController()->GetCamera()->SetYaw(48.8);
	GetCameraController()->GetCamera()->SetPitch(-27.8);

	GetCameraController()->GetCamera()->SetTranslation(glm::vec3(15000, 100.0f, 15000));
}

void CSceneWoW::TestDeleteMap()
{
	GetRootNode3D()->RemoveChild(map);

	_ASSERT(map.use_count() == 1);
	map.reset();
}

