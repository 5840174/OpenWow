#include "stdafx.h"

// Gerenal
#include "SceneWoW.h"

// Additional
#include "Client/Client.h"
#include "zlib/zlib.h"
#pragma comment(lib, "zlib.lib")

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

	m_WoWClient = std::make_unique<CWoWClient>("127.0.0.1");
	m_WoWClient->AddWorldHandler(SMSG_CHAR_ENUM, std::bind(&CSceneWoW::S_CharsEnum, this, std::placeholders::_1));
	m_WoWClient->AddWorldHandler(SMSG_LOGIN_VERIFY_WORLD, std::bind(&CSceneWoW::S_Login_Verify_World, this, std::placeholders::_1));
	m_WoWClient->AddWorldHandler(SMSG_MONSTER_MOVE, std::bind(&CSceneWoW::S_MonsterMove, this, std::placeholders::_1));
	m_WoWClient->AddWorldHandler(SMSG_COMPRESSED_UPDATE_OBJECT, std::bind(&CSceneWoW::S_SMSG_COMPRESSED_UPDATE_OBJECT, this, std::placeholders::_1));
	m_WoWClient->AddWorldHandler(SMSG_UPDATE_OBJECT, std::bind(&CSceneWoW::S_SMSG_UPDATE_OBJECT, this, std::placeholders::_1));
	m_WoWClient->BeginConnect("admin", "admin");

	map = GetRootNode3D()->CreateSceneNode<CMap>(GetBaseManager(), GetRenderDevice());

	//Load3D();
	LoadUI();


	std::shared_ptr<BuildRenderListPassTemplated<CWMO_Group_Instance>> wmoListPass = std::make_shared<BuildRenderListPassTemplated<CWMO_Group_Instance>>(GetRenderDevice(), shared_from_this());
	std::shared_ptr<BuildRenderListPassTemplated<CM2_Base_Instance>> m2ListPass = std::make_shared<BuildRenderListPassTemplated<CM2_Base_Instance>>(GetRenderDevice(), shared_from_this());

	m_Technique3D.AddPass(GetBaseManager().GetManager<IRenderPassFactory>()->CreateRenderPass("ClearPass", GetRenderDevice(), GetRenderWindow()->GetRenderTarget(), &GetRenderWindow()->GetViewport(), shared_from_this()));
	m_Technique3D.AddPass(wmoListPass);
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

void CSceneWoW::Finalize()
{
	// Insert code here

	SceneBase::Finalize();
}


void CSceneWoW::S_CharsEnum(CServerPacket & _buff)
{
	uint8 charCnt;
	_buff >> charCnt;

	std::vector<CInet_CharacterTemplate> characters;
	for (uint8 i = 0; i < charCnt; i++)
	{
		CInet_CharacterTemplate character(_buff);
		characters.push_back(character);
	}

	if (characters.empty())
		return;

	CClientPacket p(CMSG_PLAYER_LOGIN);
	p << characters[0].GUID;
	m_WoWClient->SendPacket(p);
}

void CSceneWoW::S_Login_Verify_World(CServerPacket & Buffer)
{
	uint32 mapID;
	Buffer >> mapID;

	float positionX;
	Buffer >> positionX;

	float positionY;
	Buffer >> positionY;

	float positionZ;
	Buffer >> positionZ;

	float orientation;
	Buffer >> orientation;

	_ASSERT(Buffer.isEof());

	glm::vec3 position = fromGameToReal(glm::vec3(positionX, positionY, positionZ));

	const float x = 40;
	const float y = 27;
	map->MapPreLoad(GetBaseManager().GetManager<CDBCStorage>()->DBC_Map()[mapID]);
	map->MapLoad();
	map->MapPostLoad();
	map->EnterMap(position);

	GetCameraController()->GetCamera()->SetTranslation(position);
	GetCameraController()->GetCamera()->SetYaw(48.8);
	GetCameraController()->GetCamera()->SetPitch(-27.8);
}

void CSceneWoW::S_MonsterMove(CServerPacket & Buffer)
{
	uint64 guid;
	Buffer.ReadPackedUInt64(guid);

	//Log::Green("GUID is '%s'", GetLogNameForGuid(guid));

	float positionX;
	Buffer >> positionX;
	float positionY;
	Buffer >> positionY;
	float positionZ;
	Buffer >> positionZ;

	uint32 msTime;
	Buffer >> msTime;

	uint8 isStopped;
	Buffer >> isStopped;

	uint32 movementsFlags;
	Buffer >> movementsFlags;

	uint32 timeBetweenPoints;
	Buffer >> timeBetweenPoints;

	uint32 PointsCnt;
	Buffer >> PointsCnt;

	glm::vec3 NextPoint;
	for (size_t i = 0; i < PointsCnt; i++)
	{
		float positionX1;
		Buffer >> positionX1;
		float positionY1;
		Buffer >> positionY1;
		float positionZ1;
		Buffer >> positionZ1;

		NextPoint = fromGameToReal(glm::vec3(positionX1, positionY1, positionZ1));
	}


	glm::vec3 position = fromGameToReal(glm::vec3(positionX, positionY, positionZ));

	//Log::Info("Monster move [%u] (%f, %f, %f) time [%d]", guid, position.x, position.y, position.z);

	UpdateGUIDPos(guid, position);

	m_WoWClient->getClientCache().SendQueryResponce(guid);
}

SUpdateData updData;

void CSceneWoW::S_SMSG_COMPRESSED_UPDATE_OBJECT(CServerPacket & Buffer)
{
	uint32 dataSize;
	Buffer >> dataSize;

	CByteBuffer uncompressed;
	uncompressed.writeDummy(dataSize);

	uLongf uncompressedSize;
	if (uncompress(uncompressed.getDataEx(), &uncompressedSize, Buffer.getDataFromCurrent(), Buffer.getSize() - 4) != Z_OK)
	{
		Log::Error("SMSG_COMPRESSED_UPDATE_OBJECT: Error while uncompress object.");
		return;
	}
	_ASSERT(dataSize == uncompressedSize);

	uncompressed.seek(0);
	updData.Fill(uncompressed);
}

void CSceneWoW::S_SMSG_UPDATE_OBJECT(CServerPacket & Buffer)
{
	updData.Fill(Buffer);
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
	skyManager = GetRootNode3D()->CreateSceneNode<SkyManager>(GetRenderDevice());
	skyManager->Load(1);

#if 0
	auto wmo = GetBaseManager().GetManager<IWMOManager>()->Add(GetRenderDevice(), "World\\wmo\\Azeroth\\Buildings\\Stormwind\\Stormwind.wmo");
	wmoInstance = GetRootNode3D()->CreateSceneNode<CWMO_Base_Instance>(*wmo);
	wmoInstance->SetTranslate(glm::vec3(100, 500, 100));
	wmoInstance->SetRotation(glm::vec3(0.0f, glm::pi<float>(), 0.0f));
	GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(wmoInstance.get());

	GetCameraController()->GetCamera()->SetTranslation(glm::vec3());
	GetCameraController()->GetCamera()->SetYaw(48.8);
	GetCameraController()->GetCamera()->SetPitch(-27.8);
#endif


	
	
#if 1
	const float x = 40;
	const float y = 27;
	map->MapPreLoad(GetBaseManager().GetManager<CDBCStorage>()->DBC_Map()[1]);
	map->MapLoad();
	map->MapPostLoad();
	map->EnterMap(40, 27);

	GetCameraController()->GetCamera()->SetTranslation(glm::vec3(x * C_TileSize + C_TileSize / 2.0f, 100.0f, y * C_TileSize + C_TileSize / 2.0f));
	GetCameraController()->GetCamera()->SetYaw(48.8);
	GetCameraController()->GetCamera()->SetPitch(-27.8);
#endif


}

void CSceneWoW::Load3D_M2s()
{
	CWorldObjectCreator creator(GetBaseManager());

	const auto& records = GetBaseManager().GetManager<CDBCStorage>()->DBC_CreatureDisplayInfo().Records();
	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < 3; j++)
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

			auto creature = creator.BuildCreatureFromDisplayInfo(GetRenderDevice(), this, id, nullptr);
			if (creature != nullptr)
			{
				creature->SetTranslate(glm::vec3(i * 2.5f, 0.0f, j * 2.5f));
				GetRootNode3D()->AddChild(creature);
			}
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

void CSceneWoW::UpdateGUIDPos(uint64 GUID, const glm::vec3& Position, glm::vec3 Direction)
{
	/*
	const auto& it = m_Objects.find(GUID);
	if (it == m_Objects.end())
	{
		CWorldObjectCreator creator(GetBaseManager());
		auto creature = creator.BuildCreatureFromDisplayInfo(GetRenderDevice(), this, 6910, GetRootNode3D());
		if (creature != nullptr)
		{
			creature->SetTranslate(Position);
			//creature->SetRotation(Direction);
			//creature->SetParent(GetRootNode3D().get());
		}
		m_Objects.insert(std::make_pair(GUID, creature));
	}
	else
	{
		// Recreate object if need
		const auto& crEntryName = m_EntriesName.find(GUID_ENPART(GUID));
		if (crEntryName != m_EntriesName.end())
		{
			if (it->second->GetName() != crEntryName->second.name)
			{
				auto parent = it->second->GetParent().lock();
				if (parent)
					parent->RemoveChild(it->second);

				CWorldObjectCreator creator(GetBaseManager());
				auto creature = creator.BuildCreatureFromDisplayInfo(GetRenderDevice(), this, crEntryName->second.modelID, it->second->GetParent().lock());
				if (creature != nullptr)
				{
					creature->SetTranslate(Position);
					//creature->SetRotation(Direction);
					//creature->SetParent(GetRootNode3D().get());
					m_Objects[GUID] = creature;

					return;
				}
			}
		}

		it->second->SetTranslate(Position);
	}
	*/
}
