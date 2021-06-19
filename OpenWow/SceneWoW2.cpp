#include "stdafx.h"

#if 0

// Gerenal
#include "SceneWoW2.h"

CSceneWoW2::CSceneWoW2(IBaseManager& BaseManager, const std::shared_ptr<ISceneNode>& Fake3DRootNode, const std::shared_ptr<ICameraComponent3D>& ExternalCamera)
	: SceneBase(BaseManager)
	, m_Fake3DRootNode(Fake3DRootNode)
	, m_FakeCamera(ExternalCamera)
{}

CSceneWoW2::~CSceneWoW2()
{
	OutputDebugStringW(L"Destroyed.");
}


//
// IGameState
//
void CSceneWoW2::Initialize()
{
	SceneBase::Initialize();

	m_RootNode3D = m_Fake3DRootNode;

	auto cameraNode = GetRootNode3D()->CreateSceneNode<SceneNode3D>();
	cameraNode->AddComponent(std::make_shared<CCameraComponent3D>(*cameraNode));

	SetCameraController(std::make_shared<CFreeCameraController>());
	GetCameraController()->SetCamera(cameraNode->GetComponent<ICameraComponent3D>());
	GetCameraController()->GetCamera()->SetPerspectiveProjection(ICameraComponent3D::EPerspectiveProjectionHand::Right, 45.0f, static_cast<float>(GetRenderWindow()->GetWindowWidth()) / static_cast<float>(GetRenderWindow()->GetWindowHeight()), 0.5f, 10000.0f);

	GetCameraController()->GetCamera()->SetTranslation(m_FakeCamera->GetPosition());
	GetCameraController()->GetCamera()->SetYaw(200);
	GetCameraController()->GetCamera()->SetPitch(-27.8);

	std::shared_ptr<CSceneCreateTypedListsPass> sceneListPass = std::make_shared<CSceneCreateTypedListsPass>(GetRenderDevice(), shared_from_this());

	m_Technique3D.AddPass(GetBaseManager().GetManager<IRenderPassFactory>()->CreateRenderPass("ClearPass", GetRenderDevice(), GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport(), shared_from_this()));
	m_Technique3D.AddPass(sceneListPass);
	//m_Technique3D.AddPass(std::make_shared<CRenderPass_Sky>(GetRenderDevice(), sceneListPass)->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	//m_Technique3D.AddPass(std::make_shared<CRenderPass_WDL>(GetRenderDevice(), shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	m_Technique3D.AddPass(std::make_shared<CRenderPass_MapChunk>(GetRenderDevice(), sceneListPass)->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	m_Technique3D.AddPass(std::make_shared<CRenderPass_Liquid>(GetRenderDevice(), sceneListPass)->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	m_Technique3D.AddPass(std::make_shared<CRenderPass_WMO>(GetRenderDevice(), sceneListPass)->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	//m_Technique3D.AddPass(std::make_shared<CRenderPass_WMO2>(GetRenderDevice(), wmoListPass, shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	m_Technique3D.AddPass(std::make_shared<CRenderPass_M2>(GetRenderDevice(), sceneListPass, false)->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	//m_Technique3D.AddPass(std::make_shared<CRenderPass_M2_Instanced>(GetRenderDevice(), sceneListPass, true)->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	m_Technique3D.AddPass(std::make_shared<CDrawBoundingBoxPass>(GetRenderDevice(), shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
	//m_Technique3D.AddPass(std::make_shared<CMaterialParticlePass>(GetRenderDevice(), shared_from_this())->CreatePipeline(GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport()));
}

void CSceneWoW2::Finalize()
{
	// Insert code here

	SceneBase::Finalize();
}

void CSceneWoW2::OnRayIntersected(const glm::vec3& Point)
{

}



//
//
//
void CSceneWoW2::OnPreRender(RenderEventArgs& e)
{
	CMapWMOInstance::reset();
#ifdef USE_M2_MODELS
	CMapM2Instance::reset();
#endif

	SceneBase::OnPreRender(e);
}

void CSceneWoW2::OnRender(RenderEventArgs & e)
{
	e.Camera = GetCameraController()->GetCamera().get();
	e.CameraForCulling = m_FakeCamera.get();

	m_Technique3D.Render(e);
}



//
// Keyboard events
//
bool CSceneWoW2::OnWindowKeyPressed(KeyEventArgs & e)
{
	return SceneBase::OnWindowKeyPressed(e);
}

void CSceneWoW2::OnWindowKeyReleased(KeyEventArgs & e)
{
	SceneBase::OnWindowKeyReleased(e);
}

#endif