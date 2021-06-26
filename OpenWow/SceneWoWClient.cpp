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
// IGameState
//
void CSceneWoWClient::Initialize()
{
	__super::Initialize();

	// Light
	{
		auto lightNode = CreateSceneNodeT<ISceneNode>();
		lightNode->SetName("Light");
		lightNode->SetLocalPosition(glm::vec3(-50.0f, 250.0f, -50.0f));
		lightNode->SetLocalRotationDirection(glm::vec3(-0.5, -0.5f, -0.5f));
		//lightNode->SetLocalRotationEuler(glm::vec3(45.0f, -45.0f, 0.0f));

		auto lightComponent = GetBaseManager().GetManager<IObjectsFactory>()->GetClassFactoryCast<IComponentFactory>()->CreateComponentT<CLightComponent>(cSceneNodeLightComponent, *lightNode.get());
		lightComponent->SetLight(MakeShared(CLight, GetBaseManager()));
		lightComponent->GetLight()->SetCastShadows(true);
		lightComponent->GetLight()->SetType(ELightType::Spot);
		lightComponent->GetLight()->SetAmbientColor(ColorRGB(0.25f));
		lightComponent->GetLight()->SetColor(ColorRGB(1.0f, 1.0f, 1.0f));
		lightComponent->GetLight()->SetRange(1000.0f);
		lightComponent->GetLight()->SetIntensity(1.0077f);
		lightComponent->GetLight()->SetSpotlightAngle(30.0f);

		lightNode->AddComponent(cSceneNodeLightComponent, lightComponent);
	}

	// Camera
	{
		auto cameraNode = CreateSceneNodeT<ISceneNode>();
		cameraNode->SetName("Camera");
		cameraNode->AddComponentT(GetBaseManager().GetManager<IObjectsFactory>()->GetClassFactoryCast<IComponentFactory>()->CreateComponentT<ICameraComponent3D>(cSceneNodeCameraComponent, *cameraNode));

		SetCameraController(MakeShared(CFreeCameraController));
		GetCameraController()->SetCamera(cameraNode->GetComponentT<ICameraComponent3D>());
		GetCameraController()->GetCamera()->SetPerspectiveProjection(75.0f, static_cast<float>(GetRenderWindow().GetWindowWidth()) / static_cast<float>(GetRenderWindow().GetWindowHeight()), 1.0f, 2500.0f);
		GetCameraController()->GetCamera()->SetPosition(glm::vec3(100.0f));
		GetCameraController()->GetCamera()->SetYaw(225);
		GetCameraController()->GetCamera()->SetPitch(-45);
	}

	GetBaseManager().GetManager<IWoWObjectsCreator>()->InitEGxBlend(GetRenderDevice());

	m_RendererStatisticText = CreateUIControlTCast<IUIControlText>();
	m_RendererStatisticText->SetLocalPosition(glm::vec2(5.0f, 200.0f));
	m_RendererStatisticText->SetText("");
	
	m_WowClient = std::make_unique<CWoWClient>(*this, "localhost");
	m_WowClient->BeginConnect("test2", "test2");
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
	wowRenderer->Initialize(GetRenderWindow().GetRenderTarget());
	m_ForwardRenderer = wowRenderer;
	m_DefferedRenderrer = wowRenderer;

	SetRenderer(wowRenderer);
}
