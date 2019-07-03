#include "stdafx.h"

// General
#include "GameState_World.h"

// Additional

CGameState_World::CGameState_World(const IApplication * _application)
    : base(_application)
{
	m_Viewport = Viewport(0, 0, 1280.0f, 1024.0f);
}

CGameState_World::~CGameState_World()
{
}


//
// IGameState
//
bool CGameState_World::Init()
{
	std::shared_ptr<IWMOManager> wmoManager = std::make_shared<WMOsManager>();
	AddManager<IWMOManager>(wmoManager);

	std::shared_ptr<IM2Manager> m2Manager = std::make_shared<CM2_Manager>();
	AddManager<IM2Manager>(m2Manager);

	IApplication& app = Application::Get();
	std::shared_ptr<IRenderDevice> renderDevice = app.GetRenderDevice();

	renderDevice->CreateTexture2D("Textures\\ShaneCube.blp"); // DXT1
	renderDevice->CreateTexture2D("Textures\\Minimap\\00b445de1413eeca80cc683deb9af58b.blp"); // DXT1A
	renderDevice->CreateTexture2D("Textures\\MinimapMask.blp"); // DXT3
	renderDevice->CreateTexture2D("Textures\\SpellChainEffects\\VR_Chain.blp"); // DXT5
	renderDevice->CreateTexture2D("Textures\\Moon02Glare.blp"); // RAW0
	renderDevice->CreateTexture2D("Textures\\ShadowBlob.blp"); // RAW1
	renderDevice->CreateTexture2D("Textures\\moon.blp"); // RAW8
	renderDevice->CreateTexture2D("Textures\\SunGlare.blp"); // PURE

	//
	// Camera controller
	//
	SetCameraController(std::make_shared<CFreeCameraController>());
	GetCameraController()->GetCamera()->SetViewport(m_Viewport);
    GetCameraController()->GetCamera()->SetProjectionRH(45.0f, 1280.0f / 1024.0f, 0.5f, 4000.0f);

	m_FrameQuery = renderDevice->CreateQuery(Query::QueryType::Timer, 1);

	Load3D();
	LoadUI();

	return base::Init();
}

void CGameState_World::Destroy()
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

void CGameState_World::OnResize(ResizeEventArgs & e)
{
    if (e.Width == 0 || e.Height == 0)
    {
        return;
    }

    GetCameraController()->GetCamera()->SetProjectionRH(45.0f, static_cast<float>(e.Width) / static_cast<float>(e.Height), 0.5f, 4000.0f);

    base::OnResize(e);

    GetCameraController()->GetCamera()->SetViewport(m_Viewport);

    m_3DTechnique.UpdateViewport(m_Viewport);
    m_UITechnique.UpdateViewport(m_Viewport);
}

void CGameState_World::OnPreRender(RenderEventArgs& e)
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

	CMapWMOInstance::reset();
	CMapM2Instance::reset();
}

void CGameState_World::OnRender(RenderEventArgs& e)
{
	e.Camera = GetCameraController()->GetCamera().get();
	Application::Get().GetLoader()->SetCamera(GetCameraController()->GetCamera());

    m_3DTechnique.Render(e);

    m_3D2Technique.Render(e);
}

void CGameState_World::OnPostRender(RenderEventArgs& e)
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
		if (m_MapController != nullptr)
			title += "_" + std::to_string(m_MapController->GetCurrentX()) + "_" + std::to_string(m_MapController->GetCurrentZ());

		Application::Get().GetRenderWindow()->SetWindowName(title);
	}
}

void CGameState_World::OnRenderUI(RenderEventArgs& e)
{
	e.Viewport = &m_Viewport;

	m_UITechnique.Render(e);
}

//
//
//

void CGameState_World::Load3D()
{
	IApplication& app = Application::Get();
	std::shared_ptr<IRenderDevice> renderDevice = app.GetRenderDevice();
	std::shared_ptr<RenderWindow> renderWindow = app.GetRenderWindow();

    time_t programstart;
    time(&programstart);

    Random r(programstart);

	const float x = 40;
	const float y = 29;

	// Mapý
    
	m_MapController = m_3DScene->GetRootNode()->CreateSceneNode<CMap>();
	m_MapController->MapPreLoad(DBC_Map[1]);
	m_MapController->MapLoad();
	m_MapController->MapPostLoad();
	m_MapController->EnterMap(x, y);

    GetCameraController()->GetCamera()->SetTranslate(vec3(x * C_TileSize, 200, y * C_TileSize));
    

	const uint32 cnt = 10;
	/*std::shared_ptr<Character> m_CharExtra[cnt * cnt];

	std::vector<uint32> exists;
	for (int i = 0; i < cnt; i++)
	{
		for (int j = 0; j < cnt; j++)
		{
			int index = i + j * cnt;
			m_CharExtra[index] = m_3DScene->CreateSceneNode<Character>(m_3DScene->GetRootNode());

			while (true)
			{
				int random = static_cast<int>(r.NextFloat() * 32000.0f);

				std::shared_ptr<DBC_CreatureDisplayInfoRecord> rec = DBC_CreatureDisplayInfo[random];
				if (rec == nullptr)	continue;

                std::shared_ptr<DBC_CreatureDisplayInfoExtraRecord> exRec = rec->Get_HumanoidData();
				if (exRec == nullptr) continue;

                //if (exRec->Get_Race()->Get_ID() != 1) continue;

				//if (exRec->Get_Race()->Get_ID() > 10) continue;


				if (std::find(exists.begin(), exists.end(), random) != exists.end()) continue;


				m_CharExtra[index]->InitFromDisplayInfo(random);
				m_CharExtra[index]->CreateInstances();
				m_CharExtra[index]->GetComponent<CTransformComponent>()->SetScale(vec3(5.0f));

				exists.push_back(random);
				break;
			}

			m_CharExtra[index]->GetComponent<CTransformComponent>()->SetTranslate(vec3(i * 10.0f, 0.0f, j * 10.0f));
		}
	}*/

	/*std::shared_ptr<Creature> m_Char[cnt * cnt];
	std::vector<uint32> exists;
	for (int i = 0; i < cnt; i++)
	{
		for (int j = 0; j < cnt; j++)
		{
			int index = i + j * cnt;
			m_Char[index] = std::make_shared<Creature>();
			//m_Char->InitDefault();

			while (true)
			{
				int random = static_cast<int>(r.NextFloat() * 32000.0f);
				std::shared_ptr<const DBC_CreatureDisplayInfoRecord> rec = DBC_CreatureDisplayInfo[random];
				if (rec == nullptr)	continue;

				if (rec->Get_HumanoidData() != nullptr) continue;
				if (std::find(exists.begin(), exists.end(), random) != exists.end()) continue;

				m_Char[index]->InitFromDisplayInfo(random);
				m_Char[index]->SetParent(m_3DScene->GetRootNode());
				m_Char[index]->CreateInstances();
				m_Char[index]->SetTranslate(vec3(i * 15.0f, 0.0f, j * 15.0f));

				exists.push_back(random);
				break;
			}
		}
	}*/


	/*CharacterTemplate tempPala;
	tempPala.TemplateFillDefaultPaladin();

	CharacterTemplate tempShaman;
	tempShaman.TemplateFillDefaultShaman();

	std::shared_ptr<Character> character  = std::make_shared<Character>();
	character->InitFromTemplate(tempPala);
	character->CreateInstances();
	character->SetParent(m_3DScene->GetRootNode());
	character->SetTranslate(vec3(0, 15, 0));
	character->SetTranslate(vec3(x * C_TileSize, 200, y * C_TileSize));
	character->SetScale(vec3(10.0f));
	character->GetLocalTransform();*/


    m_3DTechnique.AddPass(std::make_shared<ClearRenderTargetPass>(renderWindow->GetRenderTarget(), ClearFlags::All, g_ClearColor, 1.0f, 0));
	AddSkyPasses(renderDevice, renderWindow->GetRenderTarget(), &m_3DTechnique, &m_Viewport, m_3DScene);
	AddWDLPasses(renderDevice, renderWindow->GetRenderTarget(), &m_3DTechnique, &m_Viewport, m_3DScene);
	AddMCNKPasses(renderDevice, renderWindow->GetRenderTarget(), &m_3DTechnique, &m_Viewport, m_3DScene);
	AddWMOPasses(renderDevice, renderWindow->GetRenderTarget(), &m_3DTechnique, &m_Viewport, m_3DScene);
	AddLiquidPasses(renderDevice, renderWindow->GetRenderTarget(), &m_3DTechnique, &m_Viewport, m_3DScene);
	AddM2Passes(renderDevice, renderWindow->GetRenderTarget(), &m_3DTechnique, &m_Viewport, m_3DScene);

    /*
    // Color buffer (Color0)
    Texture::TextureFormat colorTextureFormat
    (
        Texture::Components::RGBA,
        Texture::Type::UnsignedNormalized,
        1,
        8, 8, 8, 8, 0, 0
    );
    std::shared_ptr<Texture> colorTexture = _RenderDevice->CreateTexture2D(320, 240, 1, colorTextureFormat);

    // Depth/stencil buffer
    Texture::TextureFormat depthStencilTextureFormat(
        Texture::Components::DepthStencil,
        Texture::Type::UnsignedNormalized,
        1,
        0, 0, 0, 0, 24, 8);
    std::shared_ptr<Texture> depthStencilTexture = _RenderDevice->CreateTexture2D(320, 240, 1, depthStencilTextureFormat);

    m_RenderTarget = _RenderDevice->CreateRenderTarget();
    m_RenderTarget->AttachTexture(IRenderTarget::AttachmentPoint::Color0, colorTexture);
    m_RenderTarget->AttachTexture(IRenderTarget::AttachmentPoint::DepthStencil, depthStencilTexture);

    m_Viewport2 = Viewport(0, 0, 320.0f, 240.0f);

    m_3D2Technique.AddPass(std::make_shared<ClearRenderTargetPass>(m_RenderTarget, ClearFlags::All, g_ClearColor, 1.0f, 0));
    AddMCNKPasses(renderDevice, m_RenderTarget, &m_3D2Technique, &m_Viewport2, m_3DScene);
    AddWMOPasses(renderDevice, m_RenderTarget, &m_3D2Technique, &m_Viewport2, m_3DScene);
    AddLiquidPasses(renderDevice, m_RenderTarget, &m_3D2Technique, &m_Viewport2, m_3DScene);
    AddM2Passes(renderDevice, m_RenderTarget, &m_3D2Technique, &m_Viewport2, m_3DScene);
    */

	Light dir;
	dir.m_Enabled = true;
	dir.m_Type = Light::LightType::Directional;
	dir.m_DirectionWS = vec4(0.0, -1.5f, 0.0f, 0.0f);
	dir.m_Color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	dir.m_Intensity = 0.5f;

	m_DirLight = std::make_shared<CLight3D>(dir);
	//m_MapController->AddLight(m_DirLight);
}

void CGameState_World::LoadUI()
{
	IApplication& app = Application::Get();
	std::shared_ptr<IRenderDevice> renderDevice = app.GetRenderDevice();


	// Font
	m_CameraPosText = m_UIScene->GetRootNode()->CreateSceneNode<CUITextNode>();
	m_CameraPosText->SetText("Camera position");
	m_CameraPosText->GetComponent<CTransformComponentUI>()->SetTranslate(vec2(0.0f, 0.0f));

	m_CameraRotText = m_UIScene->GetRootNode()->CreateSceneNode<CUITextNode>();
	m_CameraRotText->SetText("Camera rotation");
	m_CameraRotText->GetComponent<CTransformComponentUI>()->SetTranslate(vec2(0.0f, 20.0f));


    //std::shared_ptr<CUITextureNode> node4 = m_UIScene->GetRootNode()->CreateSceneNode<CUITextureNode>();
    //node4->SetTexture(m_RenderTarget->GetTexture(IRenderTarget::AttachmentPoint::Color0));
    //node4->GetComponent<CTransformComponentUI>()->SetTranslate(vec2(200.0f, 200.0f));
    //node4->GetComponent<CTransformComponentUI>()->SetScale(vec2(320.0f, 240.0f));

	// Texture
	if (m_MapController != nullptr)
	{
		/*std::shared_ptr<UITexture> node3 = std::make_shared<UITexture>();
		node3->SetParent(m_UIScene->GetRootNode());
		node3->SetTexture(m_MapController->getMinimap());
		node3->SetTranslate(vec2(100.0f, 100.0f));
		node3->SetScale(vec2(512.0f, 512.0f));*/
	}

	// Texture 2
	//std::shared_ptr<UITexture> node4 = std::make_shared<UITexture>();
	//node4->SetParent(m_UIScene->GetRootNode());
	//node4->SetTexture(renderDevice->CreateTexture2D("Textures\\Moon02Glare.blp"));
	//node4->SetTranslate(vec2(200.0f, 000.0f));
	//node4->SetScale(vec2(100.0f, 100.0f));


	// Color quad
	//std::shared_ptr<SceneNodeUI> node2 = std::make_shared<SceneNodeUI>();
	//node2->SetParent(m_UIScene->GetRootNode());
	//node2->SetMesh(__geom);
	//node2->SetTranslate(vec2(180.0f, 180.0f));
	//node2->SetScale(vec2(10.0f, 10.0f));


    //m_UITechnique.AddPass(std::make_shared<ClearRenderTargetPass>(app.GetRenderWindow()->GetRenderTarget(), ClearFlags::DepthStencil, g_ClearColor, 1.0f, 0));

	//
	// UI Passes
	//
	AddUIPasses(renderDevice, app.GetRenderWindow()->GetRenderTarget(), &m_UITechnique, m_Viewport, m_UIScene);
}

void CGameState_World::UpdateLights()
{
	glm::mat4 viewMatrix = GetCameraController()->GetCamera()->GetViewMatrix();

	DayNightPhase& phase = m_MapController->getDayNightPhase();
	Light& light = m_DirLight->getLight();

	light.m_DirectionWS = vec4(phase.dayDir, 0.0f);
	light.m_AmbientColor = vec4(phase.ambientColor, phase.ambientIntensity);
	light.m_Color = vec4(phase.dayColor, 1.0f) * phase.dayIntensity;
	light.m_PositionVS = viewMatrix * glm::vec4(light.m_PositionWS.xyz(), 1);
	light.m_DirectionVS = glm::normalize(viewMatrix * glm::vec4(light.m_DirectionWS.xyz(), 0));
}
