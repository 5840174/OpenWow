#include "stdafx.h"

// Gerenal
#include "SceneWoWClient.h"

CSceneWoWClient::CSceneWoWClient(IBaseManager& BaseManager, IRenderWindow& RenderWindow)
	: SceneBase(BaseManager, RenderWindow)
{
}

CSceneWoWClient::~CSceneWoWClient()
{
	Log::Info("Scene destroyed.");
}



//
// CSceneWoWClient
//
void CSceneWoWClient::SetMainMenu()
{
	auto m2Model = GetBaseManager().GetManager<IWoWObjectsCreator>()->LoadM2(GetRenderDevice(), "Interface\\GLUES\\MODELS\\UI_MainMenu_Northrend\\UI_MainMenu_Northrend.M2");

	m_MainMenu = GetRootSceneNode()->CreateSceneNode<CM2_Base_Instance>(m2Model);
	GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(m_MainMenu);

	while (m_MainMenu->GetState() < ILoadable::ELoadableState::Loaded)
	{
		Sleep(5);
	}

	auto m2CameraComponent = m_MainMenu->CreateCameraComponent(0);
	GetCameraController()->SetCamera(m2CameraComponent);
}

void CSceneWoWClient::RemoveMainMenu()
{

}



//
// IScene
//
void CSceneWoWClient::Initialize()
{
	__super::Initialize();

	// Light
	{
		m_DefaultLightNode = CreateSceneNodeT<ISceneNode>();
		m_DefaultLightNode->SetName("Light");
		m_DefaultLightNode->SetLocalPosition(glm::vec3(-50.0f, 250.0f, -50.0f));
		m_DefaultLightNode->SetLocalRotationDirection(glm::vec3(-0.5, -0.5f, -0.5f));
		//lightNode->SetLocalRotationEuler(glm::vec3(45.0f, -45.0f, 0.0f));

		auto lightComponent = GetBaseManager().GetManager<IObjectsFactory>()->GetClassFactoryCast<IComponentFactory>()->CreateComponentT<CLightComponent>(cSceneNodeLightComponent, *m_DefaultLightNode.get());
		lightComponent->SetLight(MakeShared(CLight, GetBaseManager()));
		lightComponent->GetLight()->SetCastShadows(false);
		lightComponent->GetLight()->SetType(ELightType::Directional);
		lightComponent->GetLight()->SetAmbientColor(ColorRGB(0.25f));
		lightComponent->GetLight()->SetColor(ColorRGB(1.0f, 1.0f, 1.0f));
		lightComponent->GetLight()->SetRange(1500.0f);
		lightComponent->GetLight()->SetIntensity(1.0077f);
		lightComponent->GetLight()->SetSpotlightAngle(30.0f);

		m_DefaultLightNode->AddComponent(cSceneNodeLightComponent, lightComponent);
	}

	// Camera
	{
		m_DefaultCameraNode = CreateSceneNodeT<ISceneNode>();
		m_DefaultCameraNode->SetName("Camera");
		m_DefaultCameraNode->AddComponentT(GetBaseManager().GetManager<IObjectsFactory>()->GetClassFactoryCast<IComponentFactory>()->CreateComponentT<ICameraComponent3D>(cSceneNodeCameraComponent, *m_DefaultCameraNode));

		SetCameraController(MakeShared(CFreeCameraController));
		GetCameraController()->SetCamera(m_DefaultCameraNode->GetComponentT<ICameraComponent3D>());
		GetCameraController()->GetCamera()->SetPerspectiveProjection(65.0f, static_cast<float>(GetRenderWindow().GetWindowWidth()) / static_cast<float>(GetRenderWindow().GetWindowHeight()), 1.0f, 2500.0f);
		GetCameraController()->GetCamera()->SetPosition(glm::vec3(10.0f));
		GetCameraController()->GetCamera()->SetYaw(225);
		GetCameraController()->GetCamera()->SetPitch(-45);
	}

	GetBaseManager().GetManager<IWoWObjectsCreator>()->InitEGxBlend(GetRenderDevice());

	m_RendererStatisticText = CreateUIControlTCast<IUIControlText>();
	m_RendererStatisticText->SetLocalPosition(glm::vec2(5.0f, 200.0f));
	m_RendererStatisticText->SetText("");
	


	//SetMainMenu();

	//m_WowClient = std::make_unique<CowClient>(*this, "logon.wowcircle.com");
	//m_WowClient->Login("greatejudaxor", "38522137asd");

	m_WowClient = std::make_unique<CowClient>(*this, "localhost");
	m_WowClient->Login("test2", "test2");
}

void CSceneWoWClient::Finalize()
{
	SceneBase::Finalize();
}

void CSceneWoWClient::OnUpdate(UpdateEventArgs & e)
{
	CMapWMOInstance::reset();
	CMapM2Instance::reset();

	__super::OnUpdate(e);

	m_WowClient->Update(e);

	m_RendererStatisticText->SetText(GetRenderer()->GetStatisticText());

}

bool CSceneWoWClient::OnMousePressed(const MouseButtonEventArgs & e, const Ray& RayToWorld)
{
	if (e.LeftButton)
	{
		auto selectedNodes = GetFinder().FindIntersection(RayToWorld, nullptr);
		if (false == selectedNodes.empty())
		{
			for (const auto& selectedNodesIt : selectedNodes)
			{
				if (auto m2Node = std::dynamic_pointer_cast<CM2_Base_Instance>(selectedNodesIt.second))
				{
					Log::Green("Selected node '%f' = '%s'.", selectedNodesIt.first, m2Node->GetM2().getFilename().c_str());
				}
			}
		}
	}

	return false;
}

void CSceneWoWClient::OnMouseMoved(const MouseMotionEventArgs & e, const Ray & RayToWorld)
{}



//
// Keyboard events
//
bool CSceneWoWClient::OnWindowKeyPressed(KeyEventArgs & e)
{
	return SceneBase::OnWindowKeyPressed(e);
}

void CSceneWoWClient::OnWindowKeyReleased(KeyEventArgs & e)
{
	SceneBase::OnWindowKeyReleased(e);
}



//
// Protected
//
void CSceneWoWClient::InitializeRenderer()
{
	auto wowRenderer = MakeShared(CRendererWoW, GetBaseManager(), *this);
	wowRenderer->InitializeForward(GetRenderWindow().GetRenderTarget());
	m_ForwardRenderer = wowRenderer;
	m_DefferedRenderrer = wowRenderer;

	SetRenderer(wowRenderer);
}
