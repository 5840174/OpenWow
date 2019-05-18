#include "stdafx.h"

// General
#include "GameState_Client.h"

// Additional

CGameState_Client::CGameState_Client(const IApplication * Application)
    : base(Application)
{
	m_Viewport = Viewport(0, 0, 1280.0f, 1024.0f);
}

CGameState_Client::~CGameState_Client()
{
}


void CGameState_Client::S_CharEnum(CByteBuffer& _buff)
{
	uint8 charCnt;
	_buff.readBytes(&charCnt, sizeof(uint8));

	CharacterTemplate* chars = new CharacterTemplate[charCnt];
	for (uint8 i = 0; i < charCnt; i++)
	{
		chars[i].TemplateFill(_buff);
		chars[i].TemplatePrint();
	}

	// Select character
	CharacterTemplate& charTemplate = chars[0];
	vec3 charPosition = fromGameToReal(vec3(charTemplate.Y, charTemplate.X, charTemplate.Z));

	//CByteBuffer bb;
	//bb << (uint64)charTemplate.GUID;
	//m_authWorldController->getWorldSocket()->SendData(CMSG_PLAYER_LOGIN, bb);

	m_CameraController->GetCamera()->SetTranslate(charPosition);

	_RenderDevice->Lock();

	// Create character
	std::shared_ptr<Character> character = std::make_shared<Character>();
	character->InitFromTemplate(charTemplate);
	character->CreateInstances();
	character->SetParent(m_3DScene->GetRootNode());
	character->GetComponent<CTransformComponent3D>()->SetTranslate(charPosition);

	m_MapController->MapPreLoad(DBC_Map[charTemplate.MapId]);
	m_MapController->MapLoad();
	m_MapController->MapPostLoad();
	m_MapController->EnterMap(charPosition);

	_RenderDevice->Unlock();

	//Orc
	/*ByteBuffer bb;
	bb.Write("Txtyryer", 9);
	bb.Write((uint8)2);
	bb.Write((uint8)1);
	bb.Write((uint8)0);
	bb.Write((uint8)15);
	bb.Write((uint8)6);
	bb.Write((uint8)3);
	bb.Write((uint8)6);
	bb.Write((uint8)5);
	bb.Write((uint8)0);
	SendData(CMSG_CHAR_CREATE, bb);*/

}


//
// IGameState
//
bool CGameState_Client::Init()
{
	std::shared_ptr<IWMOManager> wmoManager = std::make_shared<WMOsManager>();
	AddManager<IWMOManager>(wmoManager);

	std::shared_ptr<IM2Manager> m2Manager = std::make_shared<CM2_Manager>();
	AddManager<IM2Manager>(m2Manager);


	IApplication& app = Application::Get();
	std::shared_ptr<IRenderDevice> renderDevice = app.GetRenderDevice();


	//
	// Socket controller
	//

    m_WoWClient = std::make_shared<CWoWClient>("192.168.1.50", 3724);
    m_WoWClient->BeginConnect("admin", "admin");

	//m_authWorldController->getWorldSocket()->AddHandler(SMSG_CHAR_ENUM, FUNCTION_CLASS_WA_Builder(CGameState_Client, this, S_CharEnum, CByteBuffer&));



	//
	// Camera controller
	//
	m_CameraController = std::make_shared<CFreeCameraController>();
	m_CameraController->GetCamera()->SetViewport(m_Viewport);
	m_CameraController->GetCamera()->SetProjectionRH(45.0f, 1280.0f / 1024.0f, 1.0f, 4000.0f);
	
	m_FrameQuery = renderDevice->CreateQuery(Query::QueryType::Timer, 1);

	m_3DTechnique.AddPass(std::make_shared<ClearRenderTargetPass>(app.GetRenderWindow()->GetRenderTarget(), ClearFlags::All, g_ClearColor, 1.0f, 0));

	Load3D();
	LoadUI();

	return base::Init();
}

void CGameState_Client::Destroy()
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
void CGameState_Client::OnPreRender(RenderEventArgs& e)
{
	m_FrameQuery->Begin(e.FrameCounter);

	CMapWMOInstance::reset();
	CMapM2Instance::reset();
}

void CGameState_Client::OnRender(RenderEventArgs& e)
{
	e.Camera = m_CameraController->GetCamera().operator->(); // TODO: Shit code. Refactor me.

	m_3DTechnique.Render(e);
}

void CGameState_Client::OnPostRender(RenderEventArgs& e)
{
	m_FrameQuery->End(e.FrameCounter);

	vec3 cameraTrans = m_CameraController->GetCamera()->GetTranslation();
	m_CameraPosText->SetText("Pos: " + std::to_string(cameraTrans.x) + ", " + std::to_string(cameraTrans.y) + ", " + std::to_string(cameraTrans.z));

	vec3 cameraRot = m_CameraController->GetCamera()->GetDirection();
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

void CGameState_Client::OnRenderUI(RenderEventArgs& e)
{
	e.Viewport = &m_Viewport;

	m_UITechnique.Render(e);
}

//
//
//

void CGameState_Client::Load3D()
{
	IApplication& app = Application::Get();
	std::shared_ptr<IRenderDevice> renderDevice = app.GetRenderDevice();

	const float x = 40;
	const float y = 29;
	//const float x = 29;
	//const float y = 21;


	// M2 Model
	/*std::shared_ptr<M2> model = GetManager<IM2Manager>()->Add("Creature\\ARTHASLICHKING\\ARTHASLICHKING.m2");
	std::shared_ptr<CM2_Base_Instance> inst = std::make_shared<CM2_Base_Instance>(model);
	inst->CreateInstances();
	inst->SetParent(m_3DScene->GetRootNode());
	inst->SetScale(vec3(15.0f));
	inst->GetLocalTransform();*/

	// Map
	m_MapController = std::make_shared<CMap>();
	m_MapController->SetParent(m_3DScene->GetRootNode());
	//m_MapController->MapPreLoad(*DBC_Map[1]);
	//m_MapController->MapLoad();
	//m_MapController->MapPostLoad();
	//m_MapController->EnterMap(x, y);


    // Create character
    //std::shared_ptr<Character> character = std::make_shared<Character>();
    //character->InitFromTemplate(charTemplate);
    //character->CreateInstances();
    //character->SetParent(m_3DScene->GetRootNode());


	//m_CameraController->GetCamera()->SetTranslate(vec3(x * C_TileSize, 200, y * C_TileSize));

	//
	// 3D Passes
	//
	AddSkyPasses(renderDevice, app.GetRenderWindow()->GetRenderTarget(), &m_3DTechnique, &m_Viewport, m_3DScene);
	AddWDLPasses(renderDevice, app.GetRenderWindow()->GetRenderTarget(), &m_3DTechnique, &m_Viewport, m_3DScene);
	AddDebugPasses(renderDevice, app.GetRenderWindow()->GetRenderTarget(), &m_3DTechnique, m_Viewport, m_3DScene);
	AddMCNKPasses(renderDevice, app.GetRenderWindow()->GetRenderTarget(), &m_3DTechnique, &m_Viewport, m_3DScene);
	AddWMOPasses(renderDevice, app.GetRenderWindow()->GetRenderTarget(), &m_3DTechnique, &m_Viewport, m_3DScene);
	AddLiquidPasses(renderDevice, app.GetRenderWindow()->GetRenderTarget(), &m_3DTechnique, &m_Viewport, m_3DScene);
	AddM2Passes(renderDevice, app.GetRenderWindow()->GetRenderTarget(), &m_3DTechnique, &m_Viewport, m_3DScene);
}

void CGameState_Client::LoadUI()
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

	//
	// UI Passes
	//
	AddUIPasses(renderDevice, app.GetRenderWindow()->GetRenderTarget(), &m_UITechnique, m_Viewport, m_UIScene);
}
