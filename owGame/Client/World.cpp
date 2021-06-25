#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "World.h"

// Additional
#include "Client/Templates/CharacterTemplate.h"

#include "Renderer/RenderPass_Path.h"


CWoWWorld::CWoWWorld(IScene& Scene, const std::shared_ptr<CWorldSocket>& Socket)
	: m_Scene(Scene)
	, m_Socket(Socket)
	, m_WorldObjects(*this, m_Scene)
	, m_WorldObjectUpdater(*this, m_Scene)
	, m_ClientCache(*this)
	, m_TaxiStorage(m_Scene.GetBaseManager())
	, m_TransportAnimationStorage(m_Scene.GetBaseManager())
{
	m_SkyManager = m_Scene.GetRootSceneNode()->CreateSceneNode<SkyManager>();
	m_Scene.GetBaseManager().AddManager<ISkyManager>(m_SkyManager);

	m_Map = m_Scene.GetRootSceneNode()->CreateSceneNode<CMap>();

	m_TaxiStorage.Initialize();
	m_TransportAnimationStorage.Initialize();

	// Handlers
	AddHandler(SMSG_LOGIN_VERIFY_WORLD, std::bind(&CWoWWorld::S_SMSG_LOGIN_VERIFY_WORLD, this, std::placeholders::_1));

	AddHandler(SMSG_MOTD, std::bind(&CWoWWorld::On_SMSG_MOTD, this, std::placeholders::_1));
	AddHandler(SMSG_NOTIFICATION, std::bind(&CWoWWorld::On_SMSG_NOTIFICATION, this, std::placeholders::_1));
	
	AddHandler(SMSG_TIME_SYNC_REQ, std::bind(&CWoWWorld::On_SMSG_TIME_SYNC_REQ, this, std::placeholders::_1));
	AddHandler(SMSG_MONSTER_MOVE, std::bind(&CWoWWorld::S_SMSG_MONSTER_MOVE, this, std::placeholders::_1));
	AddHandler(SMSG_DESTROY_OBJECT, std::bind(&CWoWWorld::S_SMSG_DESTROY_OBJECT, this, std::placeholders::_1));

	m_Scene.GetRenderer()->Add3DPass(MakeShared(CRenderPass_Path, m_Scene.GetBaseManager().GetApplication().GetRenderDevice(), *this)->ConfigurePipeline(m_Scene.GetRenderWindow().GetRenderTarget()));
}

CWoWWorld::~CWoWWorld()
{
}

void CWoWWorld::EnterWorld(const CInet_CharacterTemplate& SelectedCharacter)
{
	CClientPacket p(CMSG_PLAYER_LOGIN);
	p << SelectedCharacter.GUID;
	SendPacket(p);
}

void CWoWWorld::Update(const UpdateEventArgs & e)
{
	m_WorldObjects.Update(e);
}

void CWoWWorld::Accept(IWoWVisitor * WoWVisitor)
{
	m_WorldObjects.Accept(WoWVisitor);
}





void CWoWWorld::S_SMSG_LOGIN_VERIFY_WORLD(CServerPacket& Buffer)
{
	uint32 mapID;
	Buffer >> mapID;

	float positionX, positionY, positionZ, orientation;
	Buffer >> positionX;
	Buffer >> positionY;
	Buffer >> positionZ;
	Buffer >> orientation;

	_ASSERT(Buffer.isEof());

	glm::vec3 position = fromGameToReal(glm::vec3(positionX, positionY, positionZ));
	position.y += 5.0f;

	// Skies
	//skyManager->Load(mapID);

	// Map
	m_Map->MapPreLoad(m_Scene.GetBaseManager().GetManager<CDBCStorage>()->DBC_Map()[mapID]);
	m_Map->MapLoad();
	m_Map->EnterMap(position);

	// Camera
	m_Scene.GetCameraController()->GetCamera()->SetPosition(position);
	m_Scene.GetCameraController()->GetCamera()->SetYaw(glm::degrees(orientation));
	m_Scene.GetCameraController()->GetCamera()->SetPitch(0.0f);
}

void CWoWWorld::On_SMSG_MOTD(CServerPacket & Buffer)
{
	uint32 motdCount;
	Buffer >> motdCount;

	for (size_t i = 0; i < motdCount; i++)
	{
		std::string motdLine;
		Buffer.readString(&motdLine);
		Log::Green("MOTD: %s", motdLine.c_str());
	}
}

void CWoWWorld::On_SMSG_NOTIFICATION(CServerPacket & Buffer)
{
	std::string notification;
	Buffer.readString(&notification);
	Log::Green("NOTIFICATION: %s", notification.c_str());
}

void CWoWWorld::On_SMSG_TIME_SYNC_REQ(CServerPacket & Buffer)
{
	uint32 timeSyncNextCounter;
	Buffer >> timeSyncNextCounter;

	//Log::Info("On_SMSG_TIME_SYNC_REQ: timeSyncNextCounter = '%d'.", timeSyncNextCounter);
}

void CWoWWorld::S_SMSG_MONSTER_MOVE(CServerPacket & Buffer)
{
	uint64 packedGUID;
	Buffer.ReadPackedUInt64(packedGUID);

	Buffer.seekRelative(1);

	CWoWObjectGuid guid(packedGUID);
	if (guid.GetTypeId() != EWoWObjectTypeID::TYPEID_UNIT)
		throw CException("CWoWWorld::S_SMSG_MONSTER_MOVE: Movement packet accept only EWoWObjectTypeID::TYPEID_UNIT.");

	if (auto object = m_WorldObjects.GetWoWObject(guid))
	{
		auto objectAsUnit = std::dynamic_pointer_cast<WoWUnit>(object);
		objectAsUnit->ProcessMonsterMove(Buffer);
	}
}

void CWoWWorld::S_SMSG_DESTROY_OBJECT(CServerPacket & Buffer)
{
	uint64 guid;
	Buffer >> guid;
	CWoWObjectGuid objectGuid(guid);

	uint8 isOnDeath;
	Buffer >> isOnDeath;

	if (auto object = m_WorldObjects.GetWoWObject(objectGuid))
	{
		object->Destroy();
		m_WorldObjects.EraseWoWObject(object);
	}
}



//
// CWoWWorld
//
void CWoWWorld::AddHandler(Opcodes Opcode, std::function<void(CServerPacket&)> Handler)
{
	_ASSERT(Handler != nullptr);
	m_Handlers.insert(std::make_pair(Opcode, Handler));
}

bool CWoWWorld::ProcessPacket(CServerPacket& ServerPacket)
{
	const auto& handler = m_Handlers.find(ServerPacket.GetPacketOpcode());
	if (handler != m_Handlers.end())
	{
		_ASSERT(handler->second != nullptr);
		(handler->second).operator()(ServerPacket);

		if (ServerPacket.getPos() != ServerPacket.getSize())
			throw CException("CWoWWorld::ProcessPacket: Packet '%d' is not fully readed. %d of %d.", ServerPacket.GetPacketOpcode(), ServerPacket.getPos(), ServerPacket.getSize());

		return true;
	}

	return false;
}

void CWoWWorld::SendPacket(CClientPacket & Bytes)
{
	if (auto lockedSocket = m_Socket.lock())
		lockedSocket->SendPacket(Bytes);
}

std::shared_ptr<CMap> CWoWWorld::GetMap() const
{
	return m_Map;
}




#endif