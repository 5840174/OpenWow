#include "stdafx.h"

// General
#include "GameState_Menu.h"

// Additional

CGameState_Menu::CGameState_Menu(const IApplication * _application)
    : base(_application)
{
}

CGameState_Menu::~CGameState_Menu()
{
}


//
// IGameState
//
bool CGameState_Menu::Init()
{
	IApplication& app = Application::Get();
	std::shared_ptr<IRenderDevice> renderDevice = app.GetRenderDevice();
	std::shared_ptr<RenderWindow> renderWindow = app.GetRenderWindow();

	//
	// Camera controller
	//
	SetCameraController(std::make_shared<CFreeCameraController>());
	GetCameraController()->GetCamera()->SetViewport(renderWindow->GetViewport());
    GetCameraController()->GetCamera()->SetProjectionRH(45.0f, 1280.0f / 1024.0f, 0.5f, 4000.0f);

	m_FrameQuery = renderDevice->CreateQuery(Query::QueryType::Timer, 1);

	//Load3D();
	LoadUI();

	return base::Init();
}

void CGameState_Menu::Destroy()
{
	/*renderDevice->CreateTexture2D("Textures\\ShaneCube.blp"); // DXT1
	renderDevice->CreateTexture2D("Textures\\Minimap\\00b445de1413eeca80cc683deb9af58b.blp"); // DXT1A
	renderDevice->CreateTexture2D("Textures\\MinimapMask.blp"); // DXT3
	renderDevice->CreateTexture2D("Textures\\SpellChainEffects\\VR_Chain.blp"); // DXT5
	renderDevice->CreateTexture2D("Textures\\Moon02Glare.blp"); // RAW0
	renderDevice->CreateTexture2D("Textures\\ShadowBlob.blp"); // RAW1
	renderDevice->CreateTexture2D("Textures\\moon.blp"); // RAW8
	renderDevice->CreateTexture2D("Textures\\SunGlare.blp"); // PURE*/

	base::Destroy();
}


//
//
//

void CGameState_Menu::OnResize(ResizeEventArgs & e)
{
    base::OnResize(e);
}

void CGameState_Menu::OnPreRender(RenderEventArgs& e)
{
	m_FrameQuery->Begin(e.FrameCounter);

	//if (e.FrameCounter % 20 == 0)
	//	m_MapController->getTime()->Tick();
	//UpdateLights();

	/*std::shared_ptr<ISkyManager> skyManager = GetManager<ISkyManager>();

	m_GB->GetPass()->UpdateFog(
		0.5f, //skyManager->GetFog(LightFogs::LIGHT_FOG_MULTIPLIER), 
		skyManager->GetColor(LightColors::LIGHT_COLOR_FOG), 
		skyManager->GetFog(LightFogs::LIGHT_FOG_DISTANCE)
	);*/
}

void CGameState_Menu::OnRender(RenderEventArgs& e)
{
	e.Camera = GetCameraController()->GetCamera().get();
	Application::Get().GetLoader()->SetCamera(GetCameraController()->GetCamera());

    m_3DTechnique.Render(e);
}

void CGameState_Menu::OnPostRender(RenderEventArgs& e)
{
	m_FrameQuery->End(e.FrameCounter);

	vec3 cameraTrans = GetCameraController()->GetCamera()->GetTranslation();
	m_CameraPosText->SetText("Pos: " + std::to_string(cameraTrans.x) + ", " + std::to_string(cameraTrans.y) + ", " + std::to_string(cameraTrans.z));

	vec3 cameraRot = GetCameraController()->GetCamera()->GetDirection();
	m_CameraRotText->SetText("Rot: " + std::to_string(cameraRot.x) + ", " + std::to_string(cameraRot.y) + ", " + std::to_string(cameraRot.z));

	Query::QueryResult frameResult = m_FrameQuery->GetQueryResult(e.FrameCounter - (m_FrameQuery->GetBufferCount() - 1));
	if (frameResult.IsValid)
	{
		// Frame time in milliseconds
#ifdef  IS_DX11
		m_FrameTime = frameResult.ElapsedTime * 1000.0;
#else
		m_FrameTime = frameResult.ElapsedTime / 1000000.0;
#endif

		std::string title = std::to_string(m_FrameTime);
		//Application::Get().GetRenderWindow()->SetWindowName(title);
	}
}

void CGameState_Menu::OnRenderUI(RenderEventArgs& e)
{
	e.Camera = GetCameraController()->GetCamera().get();

	m_UITechnique.Render(e);
}

//
//
//

void CGameState_Menu::Load3D()
{
	IApplication& app = Application::Get();
	std::shared_ptr<IRenderDevice> renderDevice = app.GetRenderDevice();
	std::shared_ptr<RenderWindow> renderWindow = app.GetRenderWindow();

	//
	// PreRender passes
	//

    m_3DTechnique.AddPass(std::make_shared<ClearRenderTargetPass>(renderWindow->GetRenderTarget(), ClearFlags::All, g_ClearColor));


	//
	// 3D Passes
	//
	AddSkyPasses(renderDevice, renderWindow->GetRenderTarget(), &m_3DTechnique, renderWindow->GetViewport(), m_3DScene);
	AddWDLPasses(renderDevice, renderWindow->GetRenderTarget(), &m_3DTechnique, renderWindow->GetViewport(), m_3DScene);
	AddMCNKPasses(renderDevice, renderWindow->GetRenderTarget(), &m_3DTechnique, renderWindow->GetViewport(), m_3DScene);
	AddWMOPasses(renderDevice, renderWindow->GetRenderTarget(), &m_3DTechnique, renderWindow->GetViewport(), m_3DScene);
	AddLiquidPasses(renderDevice, renderWindow->GetRenderTarget(), &m_3DTechnique, renderWindow->GetViewport(), m_3DScene);
	AddM2Passes(renderDevice, renderWindow->GetRenderTarget(), &m_3DTechnique, renderWindow->GetViewport(), m_3DScene);
}

void CGameState_Menu::LoadUI()
{
	IApplication& app = Application::Get();
	std::shared_ptr<IRenderDevice> renderDevice = app.GetRenderDevice();
    std::shared_ptr<RenderWindow> renderWindow = app.GetRenderWindow();

	// Font
	m_CameraPosText = m_UIScene->GetRootNode()->CreateSceneNode<CUITextNode>();
	m_CameraPosText->SetText("Camera position");
	m_CameraPosText->GetComponent<CTransformComponentUI>()->SetTranslate(vec2(0.0f, 0.0f));

	m_CameraRotText = m_UIScene->GetRootNode()->CreateSceneNode<CUITextNode>();
	m_CameraRotText->SetText("Camera rotation");
	m_CameraRotText->GetComponent<CTransformComponentUI>()->SetTranslate(vec2(0.0f, 20.0f));

	// Texture 2
	std::shared_ptr<CUITextureNode> node4 = m_UIScene->GetRootNode()->CreateSceneNode<CUITextureNode>();
	node4->SetTexture(renderDevice->CreateTexture2D("Textures\\Moon02Glare.blp"));
	node4->GetComponent<CTransformComponentUI>()->SetTranslate(vec2(200.0f, 000.0f));
	node4->GetComponent<CTransformComponentUI>()->SetScale(vec2(100.0f, 100.0f));


    std::shared_ptr<CUIColorNode> node5 = std::make_shared<CUIColorNode>();
    node5->SetParent(m_UIScene->GetRootNode());
    node5->GetComponent<CTransformComponentUI>()->SetTranslate(vec2(400.0f, 400.0f));
    node5->GetComponent<CTransformComponentUI>()->SetScale(vec2(100.0f, 100.0f));

	//
	// UI Passes
	//
	AddUIPasses(renderDevice, renderWindow->GetRenderTarget(), &m_UITechnique, renderWindow->GetViewport(), m_UIScene);
}
