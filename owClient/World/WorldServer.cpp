#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WorldServer.h"

// Additional
#include "Renderer/RenderPass_Path.h"

namespace
{
	const Opcodes msgMoveOpcodes[] =
	{
		MSG_MOVE_HEARTBEAT,
		MSG_MOVE_JUMP,
		MSG_MOVE_FALL_LAND,

		MSG_MOVE_START_FORWARD,
		MSG_MOVE_START_BACKWARD,
		MSG_MOVE_STOP,

		MSG_MOVE_START_STRAFE_LEFT,
		MSG_MOVE_START_STRAFE_RIGHT,
		MSG_MOVE_STOP_STRAFE,

		MSG_MOVE_START_TURN_LEFT,
		MSG_MOVE_START_TURN_RIGHT,
		MSG_MOVE_STOP_TURN,

		MSG_MOVE_START_PITCH_UP,
		MSG_MOVE_START_PITCH_DOWN,
		MSG_MOVE_STOP_PITCH,

		MSG_MOVE_TELEPORT,
		MSG_MOVE_TELEPORT_ACK,

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

	const Opcodes msgUnitsMoveOpcodes[] =
	{
		SMSG_SPLINE_SET_WALK_SPEED,
		SMSG_SPLINE_SET_RUN_SPEED,
		SMSG_SPLINE_SET_RUN_BACK_SPEED,
		SMSG_SPLINE_SET_SWIM_SPEED,
		SMSG_SPLINE_SET_SWIM_BACK_SPEED,
		SMSG_SPLINE_SET_TURN_RATE,
		SMSG_SPLINE_SET_FLIGHT_SPEED,
		SMSG_SPLINE_SET_FLIGHT_BACK_SPEED,
		SMSG_SPLINE_SET_PITCH_RATE
	};
}



CWorldServer::CWorldServer(IScene& Scene, const std::shared_ptr<CWorldSocket>& Socket)
	: m_WorldClient(Scene.GetBaseManager(), Scene)
	, m_Scene(Scene)
	, m_Socket(Socket)
	, m_WorldObjects(*this, m_Scene)
	, m_WorldObjectUpdater(*this, m_Scene)
	, m_ClientCache(*this)

{
	AddHandler(SMSG_LOGIN_VERIFY_WORLD, std::bind(&CWorldServer::S_SMSG_LOGIN_VERIFY_WORLD, this, std::placeholders::_1));

	AddHandler(SMSG_MOTD, std::bind(&CWorldServer::On_SMSG_MOTD, this, std::placeholders::_1));
	AddHandler(SMSG_NOTIFICATION, std::bind(&CWorldServer::On_SMSG_NOTIFICATION, this, std::placeholders::_1));
	
	AddHandler(SMSG_SPELL_GO, std::bind(&CWorldServer::On_SMSG_SPELL_GO, this, std::placeholders::_1));

	AddHandler(SMSG_POWER_UPDATE, std::bind(&CWorldServer::On_SMSG_POWER_UPDATE, this, std::placeholders::_1));
	AddHandler(SMSG_AURA_UPDATE, std::bind(&CWorldServer::On_SMSG_AURA_UPDATE, this, std::placeholders::_1));
	AddHandler(SMSG_AURA_UPDATE_ALL, std::bind(&CWorldServer::On_SMSG_AURA_UPDATE_ALL, this, std::placeholders::_1));
	AddHandler(SMSG_UPDATE_WORLD_STATE, std::bind(&CWorldServer::On_SMSG_UPDATE_WORLD_STATE, this, std::placeholders::_1));
	
	AddHandler(SMSG_TIME_SYNC_REQ, std::bind(&CWorldServer::On_SMSG_TIME_SYNC_REQ, this, std::placeholders::_1));
	
	AddHandler(SMSG_MONSTER_MOVE, std::bind(&CWorldServer::S_SMSG_MONSTER_MOVE, this, std::placeholders::_1, false));
	AddHandler(SMSG_MONSTER_MOVE_TRANSPORT, std::bind(&CWorldServer::S_SMSG_MONSTER_MOVE, this, std::placeholders::_1, true));

	AddHandler(SMSG_DESTROY_OBJECT, std::bind(&CWorldServer::S_SMSG_DESTROY_OBJECT, this, std::placeholders::_1));

	AddHandler(SMSG_EMOTE, std::bind(&CWorldServer::On_SMSG_EMOTE, this, std::placeholders::_1));
	AddHandler(SMSG_MESSAGECHAT, std::bind(&CWorldServer::On_SMSG_MESSAGECHAT, this, std::placeholders::_1, false));
	AddHandler(SMSG_GM_MESSAGECHAT, std::bind(&CWorldServer::On_SMSG_MESSAGECHAT, this, std::placeholders::_1, true));

	// MSG_MOVE
	{
		for (const auto& movementOpcode : msgMoveOpcodes)
			AddHandler(movementOpcode, std::bind(&CWorldServer::On_MOVE_Opcode, this, std::placeholders::_1));

		AddHandler(MSG_MOVE_TIME_SKIPPED, std::bind(&CWorldServer::On_MSG_MOVE_TIME_SKIPPED, this, std::placeholders::_1));
	}

	// SMSG_SPLINE
	for (const auto& smsgSplineOpcode : msgUnitsMoveOpcodes)
		AddHandler(smsgSplineOpcode, std::bind(&CWorldServer::On_MOVE_UnitSpeedOpcode, this, std::placeholders::_1));

	m_Scene.GetRenderer()->Add3DPass(MakeShared(CRenderPass_Path, m_Scene.GetBaseManager().GetApplication().GetRenderDevice(), *this)->ConfigurePipeline(m_Scene.GetRenderWindow().GetRenderTarget()));
}

CWorldServer::~CWorldServer()
{
}

void CWorldServer::EnterWorld(const SCharacterTemplate& SelectedCharacter)
{
	m_WorldClient.Initialize();

	CClientPacket p(CMSG_PLAYER_LOGIN);
	p << SelectedCharacter.GUID;
	SendPacket(p);
}

void CWorldServer::Update(const UpdateEventArgs & e)
{
	m_WorldObjects.Update(e);
	m_WorldClient.Update(e);
}

void CWorldServer::Accept(IWoWVisitor * WoWVisitor)
{
	m_WorldObjects.Accept(WoWVisitor);
}





void CWorldServer::S_SMSG_LOGIN_VERIFY_WORLD(CServerPacket& Buffer)
{
	uint32 mapID;
	Buffer >> mapID;

	glm::vec3 position;
	Buffer >> position;

	float orientation;
	Buffer >> orientation;

	position = fromGameToReal(position);
	position.y += 3.0f;

	m_WorldClient.EnterWorld(mapID, position);

	// Camera
	m_Scene.GetCameraController()->GetCamera()->SetPosition(position);
	m_Scene.GetCameraController()->GetCamera()->SetYaw(glm::degrees(orientation));
	m_Scene.GetCameraController()->GetCamera()->SetPitch(0.0f);
}

void CWorldServer::On_SMSG_MOTD(CServerPacket & Buffer)
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

void CWorldServer::On_SMSG_NOTIFICATION(CServerPacket & Buffer)
{
	std::string notification;
	Buffer.readString(&notification);
	Log::Green("NOTIFICATION: %s", notification.c_str());
}

namespace
{
	struct SpellCastData
	{
		/*CowGuid CasterGUID;
		CowGuid CasterUnit;
		uint8 CastID = 0;
		uint32 SpellID = 0;
		uint32 CastFlags = 0;
		uint32 CastTime = 0;
		mutable Optional<std::vector<ObjectGuid>> HitTargets;
		mutable Optional<std::vector<SpellMissStatus>> MissStatus;
		SpellTargetData Target;
		Optional<uint32> RemainingPower;
		Optional<RuneData> RemainingRunes;
		Optional<MissileTrajectoryResult> MissileTrajectory;
		Optional<SpellAmmo> Ammo;
		Optional<CreatureImmunities> Immunities;*/
	};
}

void CWorldServer::On_SMSG_SPELL_GO(CServerPacket & Buffer)
{
	Buffer.seekRelative(Buffer.getSize() - Buffer.getPos());
}

namespace
{
	enum AURA_FLAGS : uint8
	{
		AFLAG_NONE = 0x00,
		AFLAG_EFF_INDEX_0 = 0x01,
		AFLAG_EFF_INDEX_1 = 0x02,
		AFLAG_EFF_INDEX_2 = 0x04,
		AFLAG_CASTER = 0x08,
		AFLAG_POSITIVE = 0x10,
		AFLAG_DURATION = 0x20,
		AFLAG_ANY_EFFECT_AMOUNT_SENT = 0x40, // used with AFLAG_EFF_INDEX_0/1/2
		AFLAG_NEGATIVE = 0x80
	};
}

void CWorldServer::On_SMSG_POWER_UPDATE(CServerPacket & Buffer)
{
	CowGuid packedGUID;
	Buffer.ReadPackedGuid(&packedGUID);

	uint8 power;
	Buffer >> power;

	uint32 value;
	Buffer >> value;
}

void CWorldServer::On_SMSG_AURA_UPDATE(CServerPacket & Buffer)
{
	CowGuid packedGUIDTarget;
	Buffer.ReadPackedGuid(&packedGUIDTarget);

	Do_AuraUpdate(Buffer);
}

void CWorldServer::On_SMSG_AURA_UPDATE_ALL(CServerPacket & Buffer)
{
	CowGuid packedGUIDTarget;
	Buffer.ReadPackedGuid(&packedGUIDTarget);

	while (false == Buffer.isEof())
	{
		Do_AuraUpdate(Buffer);
	}
}

void CWorldServer::Do_AuraUpdate(CServerPacket & Buffer)
{
	uint8 auraSlot;
	Buffer >> auraSlot;

	uint32 auraID;
	Buffer >> auraID;
	if (auraID == 0)
		return; // Aura remove

	AURA_FLAGS auraFlags;
	Buffer >> auraFlags;

	uint8 casterLevel;
	Buffer >> casterLevel;

	uint8 stackAmountOrCharges;
	Buffer >> stackAmountOrCharges;

	if (false == (auraFlags & AFLAG_CASTER))
	{
		CowGuid packedGUIDCaster;
		Buffer.ReadPackedGuid(&packedGUIDCaster);
	}

	if (auraFlags & AFLAG_DURATION)
	{
		uint32 maxDuration, duration;
		Buffer >> maxDuration;
		Buffer >> duration;
	}
}

void CWorldServer::On_SMSG_UPDATE_WORLD_STATE(CServerPacket & Buffer)
{
	int32 variable, value;
	Buffer >> variable;
	Buffer >> value;
	//Log::Info("CWorldServer::On_SMSG_UPDATE_WORLD_STATE: World state updated. Variable '%d' = '%d'", variable, value);
}

void CWorldServer::On_SMSG_TIME_SYNC_REQ(CServerPacket & Buffer)
{
	uint32 timeSyncNextCounter;
	Buffer >> timeSyncNextCounter;

	//Log::Info("On_SMSG_TIME_SYNC_REQ: timeSyncNextCounter = '%d'.", timeSyncNextCounter);
}

void CWorldServer::S_SMSG_MONSTER_MOVE(CServerPacket & Buffer, bool Transport)
{
	CowGuid packedGUID;
	Buffer.ReadPackedGuid(&packedGUID);

	if (Transport)
	{
		CowGuid transportPackedGUID;
		Buffer.ReadPackedGuid(&transportPackedGUID);

		uint8 transportSeat;
		Buffer >> transportSeat;
	}

	Buffer.seekRelative(1);

	if (packedGUID.GetTypeId() != EWoWObjectTypeID::TYPEID_UNIT && packedGUID.GetTypeId() != EWoWObjectTypeID::TYPEID_PLAYER)
		throw CException("CWorldServer::S_SMSG_MONSTER_MOVE: Movement packet accept only TYPEID_UNIT. TypeID: '%s'.", packedGUID.GetTypeName());

	if (auto object = m_WorldObjects.Get(packedGUID))
	{
		auto objectAsUnit = std::dynamic_pointer_cast<CowServerUnit>(object);
		objectAsUnit->Do_MonsterMove(Buffer);
	}
}

void CWorldServer::S_SMSG_DESTROY_OBJECT(CServerPacket & Buffer)
{
	CowGuid objectGuid;
	Buffer >> objectGuid;

	uint8 isOnDeath;
	Buffer >> isOnDeath;

	if (auto object = m_WorldObjects.Get(objectGuid))
	{
		object->Destroy();
		m_WorldObjects.Delete(object);
	}
}

void CWorldServer::On_MOVE_Opcode(CServerPacket & Buffer)
{
	CowGuid guid;
	Buffer.ReadPackedGuid(&guid);

	if (auto object = m_WorldObjects.Get(guid))
	{
		if (auto unit = std::dynamic_pointer_cast<CowServerUnit>(object))
		{
			unit->ReadMovementInfoPacket(Buffer);

			if (Buffer.GetPacketOpcode() == MSG_MOVE_SET_WALK_SPEED)
			{
				float speed;
				Buffer >> speed;
				unit->SetSpeed(EUnitSpeed::UNIT_SPEED_WALK, speed);
			}

			if (Buffer.GetPacketOpcode() == MSG_MOVE_SET_RUN_SPEED)
			{
				float speed;
				Buffer >> speed;
				unit->SetSpeed(EUnitSpeed::UNIT_SPEED_RUN, speed);
			}

			if (Buffer.GetPacketOpcode() == MSG_MOVE_SET_RUN_BACK_SPEED)
			{
				float speed;
				Buffer >> speed;
				unit->SetSpeed(EUnitSpeed::UNIT_SPEED_RUN_BACK, speed);
			}

			if (Buffer.GetPacketOpcode() == MSG_MOVE_SET_SWIM_SPEED)
			{
				float speed;
				Buffer >> speed;
				unit->SetSpeed(EUnitSpeed::UNIT_SPEED_SWIM, speed);
			}

			if (Buffer.GetPacketOpcode() == MSG_MOVE_SET_SWIM_BACK_SPEED)
			{
				float speed;
				Buffer >> speed;
				unit->SetSpeed(EUnitSpeed::UNIT_SPEED_SWIM_BACK, speed);
			}

			if (Buffer.GetPacketOpcode() == MSG_MOVE_SET_TURN_RATE)
			{
				float speed;
				Buffer >> speed;
				unit->SetSpeed(EUnitSpeed::UNIT_SPEED_TURN_RATE, speed);
			}

			if (Buffer.GetPacketOpcode() == MSG_MOVE_SET_FLIGHT_SPEED)
			{
				float speed;
				Buffer >> speed;
				unit->SetSpeed(EUnitSpeed::UNIT_SPEED_FLIGHT, speed);
			}

			if (Buffer.GetPacketOpcode() == MSG_MOVE_SET_FLIGHT_BACK_SPEED)
			{
				float speed;
				Buffer >> speed;
				unit->SetSpeed(EUnitSpeed::UNIT_SPEED_FLIGHT_BACK, speed);
			}

			if (Buffer.GetPacketOpcode() == MSG_MOVE_SET_PITCH_RATE)
			{
				float speed;
				Buffer >> speed;
				unit->SetSpeed(EUnitSpeed::UNIT_SPEED_PITCH_RATE, speed);
			}

		}
		else
		{
			Log::Error("Type '%s'", guid.GetTypeName());
			throw CException("Oh shit2 Type");
		}
	}
	else
	{
		throw CException("Oh shit");
	}
}

void CWorldServer::On_MSG_MOVE_TIME_SKIPPED(CServerPacket & Buffer)
{
	CowGuid guid;
	Buffer.ReadPackedGuid(&guid);

	uint32 timeSkipped;
	Buffer >> timeSkipped;
}

void CWorldServer::On_MOVE_UnitSpeedOpcode(CServerPacket& Buffer)
{
	CowGuid packedGuid;
	Buffer.ReadPackedGuid(&packedGuid);

	float speed;
	Buffer >> speed;
}



//
// CWorldServer
//
void CWorldServer::AddHandler(Opcodes Opcode, std::function<void(CServerPacket&)> Handler)
{
	_ASSERT(Handler != nullptr);
	m_Handlers.insert(std::make_pair(Opcode, Handler));
}

bool CWorldServer::ProcessPacket(CServerPacket& ServerPacket)
{
	const auto& handler = m_Handlers.find(ServerPacket.GetPacketOpcode());
	if (handler != m_Handlers.end())
	{
		_ASSERT(handler->second != nullptr);
		(handler->second).operator()(ServerPacket);

		if (ServerPacket.getPos() != ServerPacket.getSize())
			throw CException("CWorldServer::ProcessPacket: Packet '%d' is not fully readed. %d of %d.", ServerPacket.GetPacketOpcode(), ServerPacket.getPos(), ServerPacket.getSize());

		return true;
	}

	return false;
}

void CWorldServer::SendPacket(CClientPacket & Bytes)
{
	if (auto lockedSocket = m_Socket.lock())
		lockedSocket->SendPacket(Bytes);
}

#endif