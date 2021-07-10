#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "World.h"

// Additional
#include "Renderer/RenderPass_Path.h"

namespace
{
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



CWoWWorld::CWoWWorld(IScene& Scene, const std::shared_ptr<CWorldSocket>& Socket)
	: m_Scene(Scene)
	, m_Socket(Socket)
	, m_WorldObjects(*this, m_Scene)
	, m_WorldObjectUpdater(*this, m_Scene)
	, m_ClientCache(*this)
	, m_TaxiStorage(m_Scene.GetBaseManager())
	, m_TransportAnimationStorage(m_Scene.GetBaseManager())
{
	m_SkyManager = m_Scene.GetRootSceneNode()->CreateSceneNode<CSkyManager>();
	m_Scene.GetBaseManager().AddManager<CSkyManager>(m_SkyManager);

	m_Map = m_Scene.GetRootSceneNode()->CreateSceneNode<CMap>();

	m_TaxiStorage.Initialize();
	m_TransportAnimationStorage.Initialize();

	// Handlers
	AddHandler(SMSG_LOGIN_VERIFY_WORLD, std::bind(&CWoWWorld::S_SMSG_LOGIN_VERIFY_WORLD, this, std::placeholders::_1));

	AddHandler(SMSG_MOTD, std::bind(&CWoWWorld::On_SMSG_MOTD, this, std::placeholders::_1));
	AddHandler(SMSG_NOTIFICATION, std::bind(&CWoWWorld::On_SMSG_NOTIFICATION, this, std::placeholders::_1));
	
	AddHandler(SMSG_SPELL_GO, std::bind(&CWoWWorld::On_SMSG_SPELL_GO, this, std::placeholders::_1));

	AddHandler(SMSG_POWER_UPDATE, std::bind(&CWoWWorld::On_SMSG_POWER_UPDATE, this, std::placeholders::_1));
	AddHandler(SMSG_AURA_UPDATE, std::bind(&CWoWWorld::On_SMSG_AURA_UPDATE, this, std::placeholders::_1));
	AddHandler(SMSG_AURA_UPDATE_ALL, std::bind(&CWoWWorld::On_SMSG_AURA_UPDATE_ALL, this, std::placeholders::_1));
	AddHandler(SMSG_UPDATE_WORLD_STATE, std::bind(&CWoWWorld::On_SMSG_UPDATE_WORLD_STATE, this, std::placeholders::_1));
	
	AddHandler(SMSG_TIME_SYNC_REQ, std::bind(&CWoWWorld::On_SMSG_TIME_SYNC_REQ, this, std::placeholders::_1));
	
	AddHandler(SMSG_MONSTER_MOVE, std::bind(&CWoWWorld::S_SMSG_MONSTER_MOVE, this, std::placeholders::_1, false));
	AddHandler(SMSG_MONSTER_MOVE_TRANSPORT, std::bind(&CWoWWorld::S_SMSG_MONSTER_MOVE, this, std::placeholders::_1, true));

	AddHandler(SMSG_DESTROY_OBJECT, std::bind(&CWoWWorld::S_SMSG_DESTROY_OBJECT, this, std::placeholders::_1));

	AddHandler(SMSG_EMOTE, std::bind(&CWoWWorld::On_SMSG_EMOTE, this, std::placeholders::_1));
	AddHandler(SMSG_MESSAGECHAT, std::bind(&CWoWWorld::On_SMSG_MESSAGECHAT, this, std::placeholders::_1, false));
	AddHandler(SMSG_GM_MESSAGECHAT, std::bind(&CWoWWorld::On_SMSG_MESSAGECHAT, this, std::placeholders::_1, true));

	// MSG_MOVE
	{
		for (const auto& movementOpcode : msgMoveOpcodes)
			AddHandler(movementOpcode, std::bind(&CWoWWorld::On_MOVE_Opcode, this, std::placeholders::_1));

		AddHandler(MSG_MOVE_TIME_SKIPPED, std::bind(&CWoWWorld::On_MSG_MOVE_TIME_SKIPPED, this, std::placeholders::_1));
	}

	// SMSG_SPLINE
	for (const auto& smsgSplineOpcode : msgUnitsMoveOpcodes)
		AddHandler(smsgSplineOpcode, std::bind(&CWoWWorld::On_MOVE_UnitSpeedOpcode, this, std::placeholders::_1));

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

namespace
{
	struct SpellCastData
	{
		/*CWoWGuid CasterGUID;
		CWoWGuid CasterUnit;
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

void CWoWWorld::On_SMSG_SPELL_GO(CServerPacket & Buffer)
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

void CWoWWorld::On_SMSG_POWER_UPDATE(CServerPacket & Buffer)
{
	CWoWGuid packedGUID;
	Buffer.ReadPackedGuid(&packedGUID);

	uint8 power;
	Buffer >> power;

	uint32 value;
	Buffer >> value;
}

void CWoWWorld::On_SMSG_AURA_UPDATE(CServerPacket & Buffer)
{
	CWoWGuid packedGUIDTarget;
	Buffer.ReadPackedGuid(&packedGUIDTarget);

	Do_AuraUpdate(Buffer);
}

void CWoWWorld::On_SMSG_AURA_UPDATE_ALL(CServerPacket & Buffer)
{
	CWoWGuid packedGUIDTarget;
	Buffer.ReadPackedGuid(&packedGUIDTarget);

	while (false == Buffer.isEof())
	{
		Do_AuraUpdate(Buffer);
	}
}

void CWoWWorld::Do_AuraUpdate(CServerPacket & Buffer)
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
		CWoWGuid packedGUIDCaster;
		Buffer.ReadPackedGuid(&packedGUIDCaster);
	}

	if (auraFlags & AFLAG_DURATION)
	{
		uint32 maxDuration, duration;
		Buffer >> maxDuration;
		Buffer >> duration;
	}
}

void CWoWWorld::On_SMSG_UPDATE_WORLD_STATE(CServerPacket & Buffer)
{
	int32 variable, value;
	Buffer >> variable;
	Buffer >> value;
	//Log::Info("CWoWWorld::On_SMSG_UPDATE_WORLD_STATE: World state updated. Variable '%d' = '%d'", variable, value);
}

void CWoWWorld::On_SMSG_TIME_SYNC_REQ(CServerPacket & Buffer)
{
	uint32 timeSyncNextCounter;
	Buffer >> timeSyncNextCounter;

	//Log::Info("On_SMSG_TIME_SYNC_REQ: timeSyncNextCounter = '%d'.", timeSyncNextCounter);
}

void CWoWWorld::S_SMSG_MONSTER_MOVE(CServerPacket & Buffer, bool Transport)
{
	CWoWGuid packedGUID;
	Buffer.ReadPackedGuid(&packedGUID);

	if (Transport)
	{
		CWoWGuid transportPackedGUID;
		Buffer.ReadPackedGuid(&transportPackedGUID);

		uint8 transportSeat;
		Buffer >> transportSeat;
	}

	Buffer.seekRelative(1);

	if (packedGUID.GetTypeId() != EWoWObjectTypeID::TYPEID_UNIT)
		throw CException("CWoWWorld::S_SMSG_MONSTER_MOVE: Movement packet accept only TYPEID_UNIT. TypeID: '%s'.", packedGUID.GetTypeName());

	if (auto object = m_WorldObjects.GetWoWObject(packedGUID))
	{
		auto objectAsUnit = std::dynamic_pointer_cast<WoWUnit>(object);
		objectAsUnit->Do_MonsterMove(Buffer);
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


namespace
{
	enum ChatMsg : uint8
	{
		CHAT_MSG_ADDON = 0xFFFFFFFF, // -1
		CHAT_MSG_SYSTEM = 0x00,
		CHAT_MSG_SAY = 0x01,
		CHAT_MSG_PARTY = 0x02,
		CHAT_MSG_RAID = 0x03,
		CHAT_MSG_GUILD = 0x04,
		CHAT_MSG_OFFICER = 0x05,
		CHAT_MSG_YELL = 0x06,
		CHAT_MSG_WHISPER = 0x07,
		CHAT_MSG_WHISPER_FOREIGN = 0x08,
		CHAT_MSG_WHISPER_INFORM = 0x09,
		CHAT_MSG_EMOTE = 0x0A,
		CHAT_MSG_TEXT_EMOTE = 0x0B,
		CHAT_MSG_MONSTER_SAY = 0x0C,
		CHAT_MSG_MONSTER_PARTY = 0x0D,
		CHAT_MSG_MONSTER_YELL = 0x0E,
		CHAT_MSG_MONSTER_WHISPER = 0x0F,
		CHAT_MSG_MONSTER_EMOTE = 0x10,
		CHAT_MSG_CHANNEL = 0x11,
		CHAT_MSG_CHANNEL_JOIN = 0x12,
		CHAT_MSG_CHANNEL_LEAVE = 0x13,
		CHAT_MSG_CHANNEL_LIST = 0x14,
		CHAT_MSG_CHANNEL_NOTICE = 0x15,
		CHAT_MSG_CHANNEL_NOTICE_USER = 0x16,
		CHAT_MSG_AFK = 0x17,
		CHAT_MSG_DND = 0x18,
		CHAT_MSG_IGNORED = 0x19,
		CHAT_MSG_SKILL = 0x1A,
		CHAT_MSG_LOOT = 0x1B,
		CHAT_MSG_MONEY = 0x1C,
		CHAT_MSG_OPENING = 0x1D,
		CHAT_MSG_TRADESKILLS = 0x1E,
		CHAT_MSG_PET_INFO = 0x1F,
		CHAT_MSG_COMBAT_MISC_INFO = 0x20,
		CHAT_MSG_COMBAT_XP_GAIN = 0x21,
		CHAT_MSG_COMBAT_HONOR_GAIN = 0x22,
		CHAT_MSG_COMBAT_FACTION_CHANGE = 0x23,
		CHAT_MSG_BG_SYSTEM_NEUTRAL = 0x24,
		CHAT_MSG_BG_SYSTEM_ALLIANCE = 0x25,
		CHAT_MSG_BG_SYSTEM_HORDE = 0x26,
		CHAT_MSG_RAID_LEADER = 0x27,
		CHAT_MSG_RAID_WARNING = 0x28,
		CHAT_MSG_RAID_BOSS_EMOTE = 0x29,
		CHAT_MSG_RAID_BOSS_WHISPER = 0x2A,
		CHAT_MSG_FILTERED = 0x2B,
		CHAT_MSG_BATTLEGROUND = 0x2C,
		CHAT_MSG_BATTLEGROUND_LEADER = 0x2D,
		CHAT_MSG_RESTRICTED = 0x2E,
		CHAT_MSG_BATTLENET = 0x2F,
		CHAT_MSG_ACHIEVEMENT = 0x30,
		CHAT_MSG_GUILD_ACHIEVEMENT = 0x31,
		CHAT_MSG_ARENA_POINTS = 0x32,
		CHAT_MSG_PARTY_LEADER = 0x33
	};

	#define MAX_CHAT_MSG_TYPE 0x34

	enum Language : int32
	{
		LANG_UNIVERSAL = 0,
		LANG_ORCISH = 1,
		LANG_DARNASSIAN = 2,
		LANG_TAURAHE = 3,
		LANG_DWARVISH = 6,
		LANG_COMMON = 7,
		LANG_DEMONIC = 8,
		LANG_TITAN = 9,
		LANG_THALASSIAN = 10,
		LANG_DRACONIC = 11,
		LANG_KALIMAG = 12,
		LANG_GNOMISH = 13,
		LANG_TROLL = 14,
		LANG_GUTTERSPEAK = 33,
		LANG_DRAENEI = 35,
		LANG_ZOMBIE = 36,
		LANG_GNOMISH_BINARY = 37,
		LANG_GOBLIN_BINARY = 38,
		LANG_ADDON = (int32)0xFFFFFFFF                        // used by addons, in 2.4.0 not exist, replaced by messagetype?
	};

	#define LANGUAGES_COUNT   19
}

void CWoWWorld::On_SMSG_EMOTE(CServerPacket & Buffer)
{
	uint32 emote;
	Buffer >> emote;
	uint64 guid;
	Buffer >> guid;
}

void CWoWWorld::On_SMSG_MESSAGECHAT(CServerPacket & Buffer, bool IsGMMessage)
{
	ChatMsg chatType;
	Buffer >> chatType;

	Language language;
	Buffer >> language;

	uint64 senderGUID;
	Buffer >> senderGUID;

	Buffer.seekRelative(4);

	std::string senderNameResult;

	switch (chatType)
	{
		case CHAT_MSG_MONSTER_SAY:
		case CHAT_MSG_MONSTER_PARTY:
		case CHAT_MSG_MONSTER_YELL:
		case CHAT_MSG_MONSTER_WHISPER:
		case CHAT_MSG_MONSTER_EMOTE:
		case CHAT_MSG_RAID_BOSS_EMOTE:
		case CHAT_MSG_RAID_BOSS_WHISPER:
		case CHAT_MSG_BATTLENET:
		{
			uint32 senderNameLenght;
			Buffer >> senderNameLenght;
			std::string senderName;
			Buffer >> senderName;
			senderNameResult = senderName;

			uint64 receiverGUID;
			Buffer >> receiverGUID;
			CWoWGuid receiverWoWGuid(receiverGUID);

			if (receiverWoWGuid && !receiverWoWGuid.IsPlayer() && !receiverWoWGuid.IsPet())
			{
				uint32 receiverNameLenght;
				Buffer >> receiverNameLenght;
				std::string receiverName;
				Buffer >> receiverName;
			}
		}
		break;
		case CHAT_MSG_WHISPER_FOREIGN:
		{
			uint32 senderNameLenght;
			Buffer >> senderNameLenght;
			std::string senderName;
			Buffer >> senderName;
			senderNameResult = senderName;

			uint64 receiverGUID;
			Buffer >> receiverGUID;
			CWoWGuid receiverWoWGuid(receiverGUID);
		}
		break;
		case CHAT_MSG_BG_SYSTEM_NEUTRAL:
		case CHAT_MSG_BG_SYSTEM_ALLIANCE:
		case CHAT_MSG_BG_SYSTEM_HORDE:
		{
			uint64 receiverGUID;
			Buffer >> receiverGUID;
			CWoWGuid receiverWoWGuid(receiverGUID);

			if (receiverWoWGuid && false == receiverWoWGuid.IsPlayer() && false == receiverWoWGuid.IsPet())
			{
				uint32 receiverNameLenght;
				Buffer >> receiverNameLenght;
				std::string receiverName;
				Buffer >> receiverName;
			}
		}
		break;
		case CHAT_MSG_ACHIEVEMENT:
		case CHAT_MSG_GUILD_ACHIEVEMENT:
		{
			uint64 receiverGUID;
			Buffer >> receiverGUID;
			CWoWGuid receiverWoWGuid(receiverGUID);
		}
		break;
		default:
		{
			if (IsGMMessage)
			{
				uint32 senderNameLenght;
				Buffer >> senderNameLenght;
				std::string senderName;
				Buffer >> senderName;
				senderNameResult = senderName;
			}

			if (chatType == CHAT_MSG_CHANNEL)
			{
				std::string channelName;
				Buffer >> channelName;
			}

			uint64 receiverGUID;
			Buffer >> receiverGUID;
			CWoWGuid receiverWoWGuid(receiverGUID);
		}
		break;
	}

	uint32 messageLenght;
	Buffer >> messageLenght;

	std::string message;
	Buffer >> message;

	uint8 chatTag;
	Buffer >> chatTag;

	if (chatType == CHAT_MSG_ACHIEVEMENT || chatType == CHAT_MSG_GUILD_ACHIEVEMENT)
	{
		uint32 achivementID;
		Buffer >> achivementID;
	}

	Log::Print("[%s]: %s", senderNameResult.c_str(), message.c_str());
}

void CWoWWorld::On_MOVE_Opcode(CServerPacket & Buffer)
{
	CWoWGuid guid;
	Buffer.ReadPackedGuid(&guid);

	if (auto object = m_WorldObjects.GetWoWObject(guid))
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
			Log::Error("Type '%s'", guid.GetTypeName());
			throw CException("Oh shit2 Type");
		}
	}
	else
	{
		throw CException("Oh shit");
	}
}

void CWoWWorld::On_MSG_MOVE_TIME_SKIPPED(CServerPacket & Buffer)
{
	CWoWGuid guid;
	Buffer.ReadPackedGuid(&guid);

	uint32 timeSkipped;
	Buffer >> timeSkipped;
}

void CWoWWorld::On_MOVE_UnitSpeedOpcode(CServerPacket& Buffer)
{
	CWoWGuid packedGuid;
	Buffer.ReadPackedGuid(&packedGuid);

	float speed;
	Buffer >> speed;
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