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
	auto m2Model = GetBaseManager().GetManager<IWoWObjectsCreator>()->LoadM2(GetRenderDevice(), "Cameras\\FlyByDeathKnight.m2");
	//auto m2Model = GetBaseManager().GetManager<IWoWObjectsCreator>()->LoadM2(GetRenderDevice(), "Cameras\\FlyByHuman.m2");

	m_WoWCameraNode = GetRootSceneNode()->CreateSceneNode<CM2_Base_Instance>(m2Model);
	GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(m_WoWCameraNode);

	while (m_WoWCameraNode->GetState() < ILoadable::ELoadableState::Loaded)
		Sleep(1);

	auto m2CameraComponent = m_WoWCameraNode->CreateCameraComponent(0);

	// Human
	//glm::vec3 origin = glm::vec3(-8945.51953125f, -118.786003112793f, 82.9306030273438f);
	//float originAngle = 0.191985994577408f;

	// Death knight
	glm::vec3 origin = glm::vec3(2301.88989257813f, -5346.31982421875f, 88.9572982788086f);
	float originAngle = 2.16421008110046;
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
		m_DefaultCameraNode = CreateSceneNodeT<ISceneNode>();
		m_DefaultCameraNode->SetName("Camera");
		m_DefaultCameraNode->AddComponentT(GetBaseManager().GetManager<IObjectsFactory>()->GetClassFactoryCast<IComponentFactory>()->CreateComponentT<ICameraComponent3D>(cSceneNodeCameraComponent, *m_DefaultCameraNode));

		SetCameraController(MakeShared(CFreeCameraController));
		GetCameraController()->SetCamera(m_DefaultCameraNode->GetComponentT<ICameraComponent3D>());
		GetCameraController()->GetCamera()->SetPerspectiveProjection(65.0f, static_cast<float>(GetRenderWindow().GetWindowWidth()) / static_cast<float>(GetRenderWindow().GetWindowHeight()), 0.5f, 2500.0f);
		GetCameraController()->GetCamera()->SetPosition(glm::vec3(10.0f));
		GetCameraController()->GetCamera()->SetYaw(225);
		GetCameraController()->GetCamera()->SetPitch(-45);
	}

	const float cMinimapOffset = 10.0f;
	minimap = GetRootUIControl()->CreateUIControl<CUIControlMinimap>();
	minimap->SetPosition(glm::vec2(GetRenderWindow().GetWindowWidth() - minimap->GetSize().x - cMinimapOffset, cMinimapOffset));
	minimap->SetOnClickCallback([this](const IUIControl* Node, glm::vec2 Point) { this->GoToCoord(Node, Point); });


	m_RendererStatisticText = CreateUIControlTCast<IUIControlText>();
	m_RendererStatisticText->SetLocalPosition(glm::vec2(5.0f, 200.0f));
	m_RendererStatisticText->SetText("");


	GetBaseManager().GetManager<IWoWObjectsCreator>()->InitEGxBlend(GetRenderDevice());


	m_WoWSkyManager = GetRootSceneNode()->CreateSceneNode<SkyManager>();
	GetBaseManager().AddManager<SkyManager>(m_WoWSkyManager);

	m_WoWMap = GetRootSceneNode()->CreateSceneNode<CMap>();

	//const float x = 31; //0 fire
	//const float y = 28; //0 fire
	//const uint32 mapID = 0;

	//const float x = 40; //1 barrens
	//const float y = 30; //1 barrens
	//const uint32 mapID = 1;

	//const float x = 26; //530 outland
	//const float y = 32; //530 outland
	//const uint32 mapID = 530;

	const float x = 30; //571 nortrend
	const float y = 21; //571 nortrend
	const uint32 mapID = 571;

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

	if (true)
	{
		m_WoWSkyManager->Load(mapID);
		

		glm::vec3 position = glm::vec3(x * C_TileSize + C_TileSize / 2.0f, 100.0f, y * C_TileSize + C_TileSize / 2.0f);

		m_WoWMap->MapPreLoad(GetBaseManager().GetManager<CDBCStorage>()->DBC_Map()[mapID]);

		minimap->SetMinimapTexture(m_WoWMap->getMinimap());

		m_WoWMap->MapLoad();
		m_WoWMap->EnterMap(position);

		GetCameraController()->GetCamera()->SetPosition(position);

		//GetCameraController()->GetCamera()->SetPosition(glm::vec3(14300, 150, 20500));
	}
	else if (false)
	{
		m_WoWSkyManager->Load(0);

		// WORLD\\WMO\\KALIMDOR\\OGRIMMAR\\OGRIMMAR.WMO

		auto wmoModel = GetBaseManager().GetManager<IWoWObjectsCreator>()->LoadWMO(GetRenderDevice(), "WORLD\\WMO\\NORTHREND\\DALARAN\\ND_DALARAN.WMO");
		
		auto wmoInstance = GetRootSceneNode()->CreateSceneNode<CWMO_Base_Instance>(wmoModel);
		GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(wmoInstance);

		GetCameraController()->GetCamera()->SetPosition(glm::vec3(0.0f));
	}
	else if (false)
	{
		m_WoWSkyManager->Load(0);

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
		m_WoWSkyManager->Load(0);

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
		m_WoWSkyManager->Load(0);

		auto creatureInstance = dynamic_cast<CWorldObjectCreator*>(GetBaseManager().GetManager<IWoWObjectsCreator>())->BuildCharacterFromDisplayInfo(GetRenderDevice(), *this, 10747);
		GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(creatureInstance);

		GetCameraController()->GetCamera()->SetPosition(glm::vec3(0.0f));
	}

	//SetMainMenu();
}

void CSceneWoW::Finalize()
{
	SceneBase::Finalize();
}

void CSceneWoW::OnUpdate(UpdateEventArgs & e)
{
	CMapWMOInstance::reset();
	CMapM2Instance::reset();

	__super::OnUpdate(e);

	m_WoWSkyManager->Update(e);
	m_WoWMap->Update(e);

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
					Log::Green("Selected node '%f' = '%s'.", selectedNodesIt.first, m2Node->getM2().getFilename().c_str());
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
	wowRenderer->Initialize(GetRenderWindow().GetRenderTarget());
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
				if (creature->getAnimator())
					creature->getAnimator()->PlayAnimation(4, true);
			}
		}
	}
}

void CSceneWoW::GoToCoord(const IUIControl* Node, const glm::vec2& Point)
{
	Log::Green("Coord %f %f", Point.x, Point.y);
	glm::vec2 conv = (Point) * (C_TileSize * 128.0f / 512.0f);

	GetCameraController()->GetCamera()->SetPosition(glm::vec3(conv.x, GetCameraController()->GetCamera()->GetPosition().y, conv.y));
}
