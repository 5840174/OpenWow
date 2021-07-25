#include "stdafx.h"

// Gerenal
#include "SceneWoW.h"

CSceneWoW::CSceneWoW(IBaseManager& BaseManager, IRenderWindow& RenderWindow)
	: SceneBase(BaseManager, RenderWindow)
{}

CSceneWoW::~CSceneWoW()
{
	GetCameraController()->SetCamera(m_DefaultCameraNode->GetComponentT<ICameraComponent3D>());
	Log::Info("Scene destroyed.");
}

void CSceneWoW::SetMainMenu()
{
	//auto m2Model = GetBaseManager().GetManager<IWoWObjectsCreator>()->LoadM2(GetRenderDevice(), "Cameras\\FlyByDeathKnight.m2");
	auto m2Model = GetBaseManager().GetManager<IWoWObjectsCreator>()->LoadM2(GetRenderDevice(), "Cameras\\FlyByHuman.m2");

	m_WoWCameraNode = GetRootSceneNode()->CreateSceneNode<CM2_Base_Instance>(m2Model);
	GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(m_WoWCameraNode);

	while (m_WoWCameraNode->GetState() < ILoadable::ELoadableState::Loaded)
		Sleep(1);

	auto m2CameraComponent = m_WoWCameraNode->CreateCameraComponent(0);

	// Human
	glm::vec3 origin = glm::vec3(-8945.51953125f, -118.786003112793f, 82.9306030273438f);
	float originAngle = 0.191985994577408f;

	// Death knight
	//glm::vec3 origin = glm::vec3(2301.88989257813f, -5346.31982421875f, 88.9572982788086f);
	//float originAngle = 2.16421008110046;
	m2CameraComponent->SetOrigin(origin, originAngle);

	GetCameraController()->SetCamera(m2CameraComponent);
}

void CSceneWoW::RemoveMainMenu()
{}


//
// IScene
//
void CSceneWoW::Initialize()
{
	__super::Initialize();

	//glm::vec3 a = glm::vec3(1, 2, 3);
	//a = Fix_From_XZmY_To_XYZ(a);
	//a = Fix_From_XYZ_to_XZmY(a);
	//Log::Print("%f %f %f", a.x, a.y, a.z);


	BoundingBox bb(glm::vec3(-1.0f, -2.0f, -3.0f), glm::vec3(4.0f, 5.0f, 6.0f));
	bb = Fix_From_XZmY_To_XYZ(bb);
	bb = Fix_From_XYZ_to_XZmY(bb);
	Log::Print("%f %f %f", bb.getMin().x, bb.getMin().y, bb.getMin().z);
	Log::Print("%f %f %f", bb.getMax().x, bb.getMax().y, bb.getMax().z);

	// Light
	{
		m_DefaultLightNode = CreateSceneNodeT<ISceneNode>();
		m_DefaultLightNode->SetName("Light");
		m_DefaultLightNode->SetLocalPosition(glm::vec3(16630, 784, 11400.0f));
		//m_DefaultLightNode->SetLocalPosition(glm::vec3(-50.0f, 250.0f, -50.0f));
		m_DefaultLightNode->SetLocalRotationDirection(glm::vec3(-0.5, -0.5f, -0.5f));
		//m_DefaultLightNode->SetLocalRotationEuler(glm::vec3(45.0f, -45.0f, 0.0f));

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
		GetCameraController()->GetCamera()->SetPerspectiveProjection(65.0f, static_cast<float>(GetRenderWindow().GetWindowWidth()) / static_cast<float>(GetRenderWindow().GetWindowHeight()), 1.0f, 1500.0f);
		GetCameraController()->GetCamera()->SetPosition(glm::vec3(10.0f));
		GetCameraController()->GetCamera()->SetYaw(225);
		GetCameraController()->GetCamera()->SetPitch(-45);
	}

	m_WorldClient = std::make_unique<CWorldClient>(GetBaseManager(), *this);

	m_DebugBall = CreateSceneNodeT<ISceneNode>();
	auto geom = GetRenderDevice().GetPrimitivesFactory().CreateSphere(5.0f);
	auto mat = MakeShared(MaterialDebug, GetRenderDevice());
	mat->SetDiffuseColor(ColorRGBA(0.0f, 1.0f, 0.0f, 1.0f));
	auto model = GetRenderDevice().GetObjectsFactory().CreateModel();
	model->AddConnection(mat, geom);
	m_DebugBall->GetComponentT<IModelComponent>()->SetModel(model);


	const float cMinimapOffset = 10.0f;
	minimap = GetRootUIControl()->CreateUIControl<CUIControlMinimap>();
	minimap->SetPosition(glm::vec2(GetRenderWindow().GetWindowWidth() - minimap->GetSize().x - cMinimapOffset, cMinimapOffset));
	minimap->SetOnClickCallback([this](const IUIControl* Node, glm::vec2 Point) { this->GoToCoord(Node, Point); });


	m_RendererStatisticText = CreateUIControlTCast<IUIControlText>();
	m_RendererStatisticText->SetLocalPosition(glm::vec2(5.0f, 200.0f));
	m_RendererStatisticText->SetText("");
	m_RendererStatisticText->SetColor(ColorRGBA(0.2f, 1.0f, 0.1f, 1.0f));


	m_AreaName = CreateUIControlTCast<IUIControlText>();
	m_AreaName->SetLocalPosition(glm::vec2(200.0f, 5.0f));
	m_AreaName->SetText("");
	m_AreaName->SetColor(ColorRGBA(0.2f, 1.0f, 0.1f, 1.0f));


	GetBaseManager().GetManager<IWoWObjectsCreator>()->InitEGxBlend(GetRenderDevice());




	const float x = 31; //0 fire
	const float y = 28; //0 fire
	const uint32 mapID = 0;

	//const float x = 40; //1 barrens
	//const float y = 30; //1 barrens
	//const uint32 mapID = 1;

	//const float x = 26; //530 outland
	//const float y = 32; //530 outland
	//const uint32 mapID = 530;

	//const float x = 30; //571 nortrend
	//const float y = 21; //571 nortrend
	//const uint32 mapID = 571;

	//const float x = 32; //571 44
	//const float y = 32; //571 44
	//const uint32 mapID = 451;

	// Ulduar
	//const float x = 0; //571 nortrend
	//const float y = 0; //571 nortrend
	//const uint32 mapID = 631;

	// Death knight
	//const float x = 0; //571 nortrend
	//const float y = 0; //571 nortrend
	//const uint32 mapID = 609;

	if (false)
	{
		glm::vec3 position = glm::vec3(x * C_TileSize + C_TileSize / 2.0f, 100.0f, y * C_TileSize + C_TileSize / 2.0f);

		m_WorldClient->EnterWorld(mapID, position);
		minimap->SetMinimapTexture(m_WorldClient->GetMap()->getMinimap());

		GetCameraController()->GetCamera()->SetPosition(position);
	}
	else if (true)
	{
		// WORLD\\WMO\\KALIMDOR\\OGRIMMAR\\OGRIMMAR.WMO
		auto wmoModel = GetBaseManager().GetManager<IWoWObjectsCreator>()->LoadWMO(GetRenderDevice(), "WORLD\\WMO\\KALIMDOR\\OGRIMMAR\\OGRIMMAR.WMO");
		//auto wmoModel = GetBaseManager().GetManager<IWoWObjectsCreator>()->LoadWMO(GetRenderDevice(), "WORLD\\WMO\\NORTHREND\\DALARAN\\ND_DALARAN.WMO");
		
		auto wmoInstance = GetRootSceneNode()->CreateSceneNode<CWMO_Base_Instance>(wmoModel);
		GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(wmoInstance);

		GetCameraController()->GetCamera()->SetPosition(glm::vec3(0.0f));
	}
	else if (false)
	{
		

		auto name = "WORLD\\GOOBER\\G_SCOURGERUNECIRCLECRYSTAL.M2";
		auto name2 = "WORLD\\EXPANSION02\\DOODADS\\NEXUS\\NEXUS_ENERGYCHAINS.M2";
		auto name3 = "WORLD\\NODXT\\GENERIC\\PASSIVEDOODADS\\VOLUMETRICLIGHTS\\LD_LIGHTSHAFT01.M2";
		auto name4 = "WORLD\\EXPANSION02\\DOODADS\\ICECROWN\\EFFECTS\\ICECROWN_ICECORE.M2";
		auto name5 = "WORLD\\EXPANSION02\\DOODADS\\DALARAN\\DALARAN_BUILDINGCRYSTAL_01.M2";
		auto name6 = "WORLD\\EXPANSION02\\DOODADS\\HOWLINGFJORD\\FOG\\HFJORD_FOG_01.M2";
		auto name7 = "WORLD\\EXPANSION02\\DOODADS\\GENERIC\\WATERFALLS\\WATERFALLS_SET2_MED_TALL_WITHCHOP.M2";
		auto name8 = "Creature\\spirithealer\\SpiritHealer.m2";
		auto name9 = "WORLD\\EXPANSION02\\DOODADS\\GENERIC\\SCOURGE\\SC_BRAZIER1.M2";
		

		auto m2Model = GetBaseManager().GetManager<IWoWObjectsCreator>()->LoadM2(GetRenderDevice(), name9);

		auto m2Instance = GetRootSceneNode()->CreateSceneNode<CM2_Base_Instance>(m2Model);
		GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(m2Instance);

		GetCameraController()->GetCamera()->SetPosition(glm::vec3(0.0f));
	}
	else if (false)
	{
		M2Test();

		GetCameraController()->GetCamera()->SetPosition(glm::vec3(0.0f));
	}
	else if (true)
	{
		const char* charString = "vmhVAAAAAABZaWVsdAACAwABBAAFBFArEQAAOwIAANhYsUUxiSlE86YiRAAAAAAAAAAAAAAAAACrXgAAUAAAAAEAAACS8gAAAQAAAAAQ+wAAAgAAAAAk/QAAAwAAAAAAAAAAAAAAAAA8/QAABQAAAAC7/AAABgAAAAA1/QAABwAAAACC/wAACAAAAAAy/QAACQAAAACQ8gAACgAAAADQmAAACwAAAADW+QAACwAAAAD0+gAADAAAAAANCgEADAAAAABG+wAAEAAAAACY+gAAEbIAAAAAAAAAAAAAAABz+wAAGgAAAAAAAAAAAAAAAADchwAAEgAAAADchwAAEgAAAADchwAAEgAAAADchwAAEgAAAAA=";
		SCharacterTemplate character;
		character.FromBase64String(charString);

		CWorldObjectCreator creator(GetBaseManager());
		auto creature = creator.BuildCharacterFromTemplate(GetRenderDevice(), *this, character);
	}
	else
	{
		auto creatureInstance = dynamic_cast<CWorldObjectCreator*>(GetBaseManager().GetManager<IWoWObjectsCreator>())->BuildCharacterFromDisplayInfo(GetRenderDevice(), *this, 10747);
		GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(creatureInstance);

		GetCameraController()->GetCamera()->SetPosition(glm::vec3(0.0f));
	}

	GetCameraController()->GetCamera()->SetYaw(-90.0f);
	GetCameraController()->GetCamera()->SetPitch(75.0f);

	//SetMainMenu();
}

void CSceneWoW::Finalize()
{
	SceneBase::Finalize();
}

void CSceneWoW::OnUpdate(UpdateEventArgs & e)
{
	__super::OnUpdate(e);

	m_WorldClient->Update(e);

	if (auto map = m_WorldClient->GetMap())
	{
		//m_AreaName->SetText();
	
		if (m_DebugBall)
		{
			auto pos = e.Camera->GetPosition();
			if (map)
				pos.y = map->GetTerrainHeight(pos);
			m_DebugBall->SetLocalPosition(pos);
		}
	}
	
	if (auto skyManager = m_WorldClient->GetSky())
	{
		const auto& currentDNPhase = skyManager->GetDNPhase();

		glm::vec3 currDir;
		ColorRGB colorAmbient = currentDNPhase.ambientColor * currentDNPhase.ambientIntensity;
		ColorRGB colorDiffuse;

		if (currentDNPhase.dayIntensity > currentDNPhase.nightIntensity)
		{
			currDir = currentDNPhase.dayDir;
			colorDiffuse = currentDNPhase.dayColor * currentDNPhase.dayIntensity;
		}
		else
		{
			currDir = currentDNPhase.nightDir;
			colorDiffuse = currentDNPhase.nightColor * currentDNPhase.nightIntensity;
		}

		colorAmbient = skyManager->GetColor(ESkyColors::SKY_COLOR_GLOBAL_AMBIENT);
		colorDiffuse = skyManager->GetColor(ESkyColors::SKY_COLOR_GLOBAL_DIFFUSE);

		m_DefaultLightNode->SetLocalRotationDirection(currDir);
		m_DefaultLightNode->GetComponentT<ILightComponent3D>()->GetLight()->SetAmbientColor(colorAmbient);
		m_DefaultLightNode->GetComponentT<ILightComponent3D>()->GetLight()->SetColor(colorDiffuse);
	}

	

	m_RendererStatisticText->SetText(GetRenderer()->GetStatisticText());
}

bool CSceneWoW::OnMousePressed(const MouseButtonEventArgs & e, const Ray& RayToWorld)
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

void CSceneWoW::OnMouseMoved(const MouseMotionEventArgs & e, const Ray & RayToWorld)
{}



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
// Protected
//
void CSceneWoW::InitializeRenderer()
{
	auto wowRenderer = MakeShared(CRendererWoW, GetBaseManager(), *this);
	wowRenderer->InitializeForward(GetRenderWindow().GetRenderTarget());
	m_ForwardRenderer = wowRenderer;
	m_DefferedRenderrer = wowRenderer;

	SetRenderer(wowRenderer);
}


void CSceneWoW::M2Test()
{
	CWorldObjectCreator creator(GetBaseManager());

	const auto& records = GetBaseManager().GetManager<CDBCStorage>()->DBC_CreatureDisplayInfo().Records();
	for (size_t i = 0; i < 25; i++)
	{
		for (size_t j = 0; j < 25; j++)
		{
			size_t id;
			while (true)
			{
				id = Random::NextUInt() % records.size();
				const auto& data = records.find(id);
				if (data != records.end())
					break;
			}

			auto creature = creator.BuildCreatureFromDisplayInfo(GetRenderDevice(), *this, id);
			if (creature != nullptr)
			{
				creature->SetPosition(glm::vec3(i * 25.0f, 0.0f, j * 25.0f));
				if (creature->GetAnimatorComponent())
					creature->GetAnimatorComponent()->PlayAnimation(EAnimationID::Stand, true);
			}
		}
	}
}

void CSceneWoW::GoToCoord(const IUIControl* Node, const glm::vec2& Point)
{
	Log::Green("Coord %f %f", Point.x, Point.y);
	glm::vec2 conv = (Point) * (C_TileSize * 128.0f / 512.0f);

	//GetBaseManager().GetManager<ILoader>()->ClearLoadQueue();
	//m_WoWMap->ClearCache();

	GetCameraController()->GetCamera()->SetPosition(glm::vec3(conv.x, GetCameraController()->GetCamera()->GetPosition().y, conv.y));
}
