#include "stdafx.h"

// Gerenal
#include "SceneWoW.h"

CSceneWoW::CSceneWoW(IBaseManager& BaseManager)
	: SceneBase(BaseManager)
{}

CSceneWoW::~CSceneWoW()
{
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

	

	Load3D_M2s();
	LoadUI();

	//cameraNode->SetTranslate(vec3(-50, 160, 170));
	//GetCameraController()->GetCamera()->SetYaw(-51);
	//GetCameraController()->GetCamera()->SetPitch(-38);


	
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
	/*
	auto wmo = GetBaseManager().GetManager<IWMOManager>()->Add(GetRenderDevice(), "World\\wmo\\Azeroth\\Buildings\\Stormwind\\Stormwind.wmo");
	wmoInstance = GetRootNode3D()->CreateSceneNode<CWMO_Base_Instance>(*wmo);
	wmoInstance->SetTranslate(glm::vec3(100, 100, 100));
	wmoInstance->RecalcVerts();
	GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(wmoInstance);


	GetCameraController()->GetCamera()->SetTranslation(vec3(500, 500, 500));
	GetCameraController()->GetCamera()->SetYaw(200);
	GetCameraController()->GetCamera()->SetPitch(-27.8);
	*/

	//auto m2 = GetBaseManager().GetManager<IM2Manager>()->Add(GetRenderDevice(), "Creature\\KelThuzad\\KelThuzad.m2");
	//m2Instance = GetRootNode3D()->CreateSceneNode<CM2_Base_Instance>(*m2);

	skyManager = GetRootNode3D()->CreateSceneNode<SkyManager>(GetRenderDevice());
	skyManager->Load(0);

	map = GetRootNode3D()->CreateSceneNode<CMap>(GetBaseManager(), GetRenderDevice());

	/*time_t t = time(0);   // get time now
	tm* now = localtime(&t);
	m_GameTime.Set(now->tm_hour, now->tm_min);*/
	wowGameTime.Set(11, 0);

	const float x = 40;
	const float y = 27;
	map->MapPreLoad(GetBaseManager().GetManager<CDBCStorage>()->DBC_Map()[1]);
	map->MapLoad();
	map->MapPostLoad();
	map->EnterMap(x, y);

	GetCameraController()->GetCamera()->SetTranslation(vec3(x * C_TileSize + C_TileSize / 2.0f, 300, y * C_TileSize + C_TileSize / 2.0f));
	GetCameraController()->GetCamera()->SetYaw(48.8);
	GetCameraController()->GetCamera()->SetPitch(-27.8);


	std::shared_ptr<BuildRenderListPassTemplated<CWMO_Group_Instance>> wmoListPass = std::make_shared<BuildRenderListPassTemplated<CWMO_Group_Instance>>(GetRenderDevice(), shared_from_this());
	std::shared_ptr<BuildRenderListPassTemplated<CM2_Base_Instance>> m2ListPass = std::make_shared<BuildRenderListPassTemplated<CM2_Base_Instance>>(GetRenderDevice(), shared_from_this());

	m_Technique3D.AddPass(GetBaseManager().GetManager<IRenderPassFactory>()->CreateRenderPass("ClearPass", GetRenderDevice(), GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport(), shared_from_this()));
	//m_Technique3D.AddPass(wmoListPass);
	//m_Technique3D.AddPass(m2ListPass);
	m_Technique3D.AddPass(std::make_shared<CRenderPass_Sky>(GetRenderDevice(), shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	//m_Technique3D.AddPass(std::make_shared<CRenderPass_WDL>(GetRenderDevice(), shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	m_Technique3D.AddPass(std::make_shared<CRenderPass_ADT_MCNK>(GetRenderDevice(), shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	m_Technique3D.AddPass(std::make_shared<CRenderPass_WMO>(GetRenderDevice(), shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	//m_Technique3D.AddPass(std::make_shared<CRenderPass_WMO2>(GetRenderDevice(), wmoListPass, shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	m_Technique3D.AddPass(std::make_shared<CRenderPass_M2>(GetRenderDevice(), shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	//m_Technique3D.AddPass(std::make_shared<CRenderPass_M2_Instanced>(GetRenderDevice(), m2ListPass, shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	m_Technique3D.AddPass(std::make_shared<CRenderPass_Liquid>(GetRenderDevice(), shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	//m_Technique3D.AddPass(std::make_shared<CDrawBoundingBoxPass>(GetRenderDevice(), shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
}

void CSceneWoW::Load3D_M2s()
{
	CWorldObjectCreator creator(GetBaseManager(), GetRenderDevice());

	const auto& records = GetBaseManager().GetManager<CDBCStorage>()->DBC_CreatureDisplayInfo().Records();
	for (size_t i = 0; i < 50; i++)
	{
		for (size_t j = 0; j < 50; j++)
		{
			size_t id = rand() % records.size();

			while (true)
			{
				const auto& data = records.find(id);
				if (data != records.end())
				{
					break;
				}

				id = rand() % records.size();
			}

			auto creature = creator.BuildCreatureFromDisplayInfo(this, id);
			creature->SetTranslate(glm::vec3(i * 5.0f, 0.0f, j * 5.0f));
			creature->SetParent(GetRootNode3D().get());
		}
	}

	GetCameraController()->GetCamera()->SetTranslation(vec3(0, 0, 0));
	GetCameraController()->GetCamera()->SetYaw(48.8);
	GetCameraController()->GetCamera()->SetPitch(-27.8);

	m_Technique3D.AddPass(GetBaseManager().GetManager<IRenderPassFactory>()->CreateRenderPass("ClearPass", GetRenderDevice(), GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport(), shared_from_this()));
	m_Technique3D.AddPass(std::make_shared<CRenderPass_M2>(GetRenderDevice(), shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
}


void CSceneWoW::LoadUI()
{
	m_TechniqueUI.AddPass(std::make_shared<CUIFontPass>(GetRenderDevice(), shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
}
