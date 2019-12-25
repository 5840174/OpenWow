#include "stdafx.h"

// Gerenal
#include "GameState_Map.h"

// Additional
#include "CreatePasses.h"

CGameState_Map::CGameState_Map(IBaseManager * BaseManager, std::shared_ptr<IRenderWindow> RenderWindow, IWindowEvents* WindowEvents)
	: CGameState(BaseManager, RenderWindow, WindowEvents)
{}

CGameState_Map::~CGameState_Map()
{}


//
// IGameState
//
bool CGameState_Map::Init()
{
	CGameState::Init();

	SetCameraController(std::make_shared<CFreeCameraController>());
	GetCameraController()->GetCamera()->SetProjection(ICamera::ProjectionHand::Right, 45.0f, GetRenderWindow()->GetWindowWidth() / GetRenderWindow()->GetWindowHeight(), 0.5f, 4000.0f);

	Load3D();
	LoadUI();

	return true;
}

void CGameState_Map::Destroy()
{
	// Insert code here

	CGameState::Destroy();
}


//
//
//

void CGameState_Map::OnResize(ResizeEventArgs& e)
{
	CGameState::OnResize(e);
}

void CGameState_Map::OnPreRender(RenderEventArgs& e)
{
	CGameState::OnPreRender(e);
}

void CGameState_Map::OnRender(RenderEventArgs& e)
{
	CGameState::OnRender(e);
}

void CGameState_Map::OnPostRender(RenderEventArgs& e)
{
	CGameState::OnPostRender(e);
}

void CGameState_Map::OnRenderUI(RenderEventArgs& e)
{
	CGameState::OnRenderUI(e);
}

//
//
//

void CGameState_Map::Load3D()
{
	const float x = 40;
	const float y = 31;

	m_MapController = m_Scene3D->GetRootNode()->CreateWrappedSceneNode<CMap>("SceneNodeBase", GetBaseManager());
	m_MapController->MapPreLoad(DBC_Map[1]);
	m_MapController->MapLoad();
	m_MapController->MapPostLoad();
	m_MapController->EnterMap(x, y);


	GetCameraController()->GetCameraMovement()->SetTranslate(vec3(x * C_TileSize + C_TileSize / 2.0f, 300, y * C_TileSize + C_TileSize / 2.0f));
	GetCameraController()->GetCameraMovement()->SetYaw(48.8);
	GetCameraController()->GetCameraMovement()->SetPitch(-27.8);

	m_Technique3D.AddPass(GetBaseManager()->GetManager<IRenderPassFactory>()->CreateRenderPass("ClearPass", GetRenderDevice(), GetRenderWindow()->GetRenderTarget(), GetRenderWindow()->GetViewport(), m_Scene3D));
	AddSkyPasses(GetRenderDevice(), GetRenderWindow()->GetRenderTarget(), &m_Technique3D, GetRenderWindow()->GetViewport(), m_Scene3D);
	AddWDLPasses(GetRenderDevice(), GetRenderWindow()->GetRenderTarget(), &m_Technique3D, GetRenderWindow()->GetViewport(), m_Scene3D);
	AddMCNKPasses(GetRenderDevice(), GetRenderWindow()->GetRenderTarget(), &m_Technique3D, GetRenderWindow()->GetViewport(), m_Scene3D);
	AddWMOPasses(GetRenderDevice(), GetRenderWindow()->GetRenderTarget(), &m_Technique3D, GetRenderWindow()->GetViewport(), m_Scene3D);
	m_Technique3D.AddPass(GetBaseManager()->GetManager<IRenderPassFactory>()->CreateRenderPass("LiquidPass", GetRenderDevice(), GetRenderWindow()->GetRenderTarget(), GetRenderWindow()->GetViewport(), m_Scene3D));
	AddM2Passes(GetRenderDevice(), GetRenderWindow()->GetRenderTarget(), &m_Technique3D, GetRenderWindow()->GetViewport(), m_Scene3D);
}

void CGameState_Map::LoadUI()
{
	m_TechniqueUI.AddPass(GetBaseManager()->GetManager<IRenderPassFactory>()->CreateRenderPass("BaseUIPass", GetRenderDevice(), GetRenderWindow()->GetRenderTarget(), GetRenderWindow()->GetViewport(), m_SceneUI));
}