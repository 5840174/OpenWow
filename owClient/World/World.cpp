#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "World.h"

// Additional
#include "Renderer/RenderPass_Path.h"

const Opcodes msgMoveOpcodes[] =
{
	MSG_MOVE_HEARTBEAT,

	MSG_MOVE_START_FORWARD,
	MSG_MOVE_START_BACKWARD,
	MSG_MOVE_STOP,

	MSG_MOVE_START_STRAFE_LEFT,
	MSG_MOVE_START_STRAFE_RIGHT,
	MSG_MOVE_STOP_STRAFE,

	MSG_MOVE_JUMP,

	MSG_MOVE_START_TURN_LEFT,
	MSG_MOVE_START_TURN_RIGHT,
	MSG_MOVE_STOP_TURN,

	MSG_MOVE_START_PITCH_UP,
	MSG_MOVE_START_PITCH_DOWN,
	MSG_MOVE_STOP_PITCH,

	MSG_MOVE_TELEPORT,
	MSG_MOVE_TELEPORT_ACK,

	MSG_MOVE_FALL_LAND,

	MSG_MOVE_START_SWIM,
	MSG_MOVE_STOP_SWIM,

	MSG_MOVE_SET_WALK_SPEED,
	MSG_MOVE_SET_RUN_SPEED,
	MSG_MOVE_SET_RUN_BACK_SPEED,
	MSG_MOVE_SET_SWIM_SPEED,
	MSG_MOVE_SET_SWIM_BACK_SPEED,
	MSG_MOVE_SET_TURN_RATE,
	MSG_MOVE_SET_FLIGHT_SPEED,
	MSG_MOVE_SET_FLIGHT_BACK_SPEED,
	MSG_MOVE_SET_PITCH_RATE,

	MSG_MOVE_SET_FACING,
	MSG_MOVE_SET_PITCH
};

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
	m_Scene.GetBaseManager().AddManager<SkyManager>(m_SkyManager);

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

	for (const auto& movementOpcode : msgMoveOpcodes)
		AddHandler(movementOpcode, std::bind(&CWoWWorld::On_MOVE_Opcode, this, std::placeholders::_1));

	m_Scene.GetRenderer()->Add3DPass(MakeShared(CRenderPass_Path, m_Scene.GetBaseManager().GetApplication().GetRenderDevice(), *this)->ConfigurePipeline(m_Scene.GetRenderWindow().GetRenderTarget()));
}

CWoWWorld::~CWoWWorld()
{
}

void CWoWWorld::EnterWorld(const SCharacterTemplate& SelectedCharacter)
{
	CClientPacket p(CMSG_PLAYER_LOGIN);
	p << SelectedCharacter.GUID;
	SendPacket(p);
}

void CWoWWorld::Update(const UpdateEventArgs & e)
{
	m_WorldObjects.Update(e);

	if (m_Map)
		m_Map->Update(e);

	if (m_SkyManager)
		m_SkyManager->Update(e);
}

void CWoWWorld::Accept(IWoWVisitor * WoWVisitor)
{
	m_WorldObjects.Accept(WoWVisitor);
}





void CWoWWorld::S_SMSG_LOGIN_VERIFY_WORLD(CServerPacket& Buffer)
{
	uint32 mapID;
	Buffer >> mapID;

	glm::vec3 position;
	Buffer >> position;
	float orientation;
	Buffer >> orientation;

	_ASSERT(Buffer.isEof());

	position = fromGameToReal(position);
	position.y += 3.0f;

	// Skies
	m_SkyManager->Load(mapID);

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

	CWoWGuid guid(packedGUID);
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
	CWoWGuid objectGuid(guid);

	uint8 isOnDeath;
	Buffer >> isOnDeath;

	if (auto object = m_WorldObjects.GetWoWObject(objectGuid))
	{
		object->Destroy();
		m_WorldObjects.EraseWoWObject(object);
	}
}

void CWoWWorld::On_MOVE_Opcode(CServerPacket & Buffer)
{
	uint64 guid;
	Buffer.ReadPackedUInt64(guid);
	CWoWGuid wowGuid(guid);

	if (auto object = m_WorldObjects.GetWoWObject(wowGuid))
	{
		if (auto unit = std::dynamic_pointer_cast<WoWUnit>(object))
		{
			unit->ReadMovementInfoPacket(Buffer);


			if (Buffer.GetPacketOpcode() == MSG_MOVE_SET_WALK_SPEED ||
				Buffer.GetPacketOpcode() == MSG_MOVE_SET_RUN_SPEED ||
				Buffer.GetPacketOpcode() == MSG_MOVE_SET_RUN_BACK_SPEED ||
				Buffer.GetPacketOpcode() == MSG_MOVE_SET_SWIM_SPEED ||
				Buffer.GetPacketOpcode() == MSG_MOVE_SET_SWIM_BACK_SPEED ||
				Buffer.GetPacketOpcode() == MSG_MOVE_SET_TURN_RATE ||
				Buffer.GetPacketOpcode() == MSG_MOVE_SET_FLIGHT_SPEED ||
				Buffer.GetPacketOpcode() == MSG_MOVE_SET_FLIGHT_BACK_SPEED ||
				Buffer.GetPacketOpcode() == MSG_MOVE_SET_PITCH_RATE)
			{
				float speed;
				Buffer >> speed;
			}
		}
		else
		{
			Log::Error("Type '%s'", wowGuid.GetTypeName());
			throw CException("Oh shit2 Type");
		}
	}
	else
	{
		throw CException("Oh shit");
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