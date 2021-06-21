#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "World.h"

// Additional
#include "Client/Templates/CharacterTemplate.h"

// Types
#include "WoWBag.h"
#include "WoWCorpse.h"
#include "WoWDynamicObject.h"
#include "WoWGameObject.h"
#include "WoWItem.h"
#include "WoWPlayer.h"
#include "WoWUnit.h"

// zlib
#include "zlib/source/zlib.h"
#pragma comment(lib, "zlib.lib")

WoWWorld::WoWWorld(IScene& Scene, const std::shared_ptr<CWorldSocket>& Socket)
	: m_Scene(Scene)
	, m_Socket(Socket)
{
	m_Socket->SetExternalHandler(std::bind(&WoWWorld::ProcessHandler, this, std::placeholders::_1, std::placeholders::_2));

	skyManager = Scene.GetRootSceneNode()->CreateSceneNode<SkyManager>();
	Scene.GetBaseManager().AddManager<ISkyManager>(skyManager);

	map = Scene.GetRootSceneNode()->CreateSceneNode<CMap>();

	// Handlers
	AddHandler(SMSG_CHAR_ENUM, std::bind(&WoWWorld::S_SMSG_CHAR_ENUM, this, std::placeholders::_1));
	AddHandler(SMSG_LOGIN_VERIFY_WORLD, std::bind(&WoWWorld::S_SMSG_LOGIN_VERIFY_WORLD, this, std::placeholders::_1));
	AddHandler(SMSG_COMPRESSED_UPDATE_OBJECT, std::bind(&WoWWorld::S_SMSG_COMPRESSED_UPDATE_OBJECT, this, std::placeholders::_1));
	AddHandler(SMSG_UPDATE_OBJECT, std::bind(&WoWWorld::S_SMSG_UPDATE_OBJECT, this, std::placeholders::_1));
	AddHandler(SMSG_MONSTER_MOVE, std::bind(&WoWWorld::S_SMSG_MONSTER_MOVE, this, std::placeholders::_1));
}

WoWWorld::~WoWWorld()
{
	/*while (m_Socket->ErasedByHandler() == false)
	{
		Log::Info("Wait for close socket...");
		Sleep(10);
	}*/
}


void WoWWorld::S_SMSG_CHAR_ENUM(CServerPacket& Bytes)
{
	uint8 charCnt;
	Bytes >> charCnt;

	std::vector<CInet_CharacterTemplate> characters;
	for (uint8 i = 0; i < charCnt; i++)
	{
		CInet_CharacterTemplate character(Bytes);
		characters.push_back(character);
	}

	if (characters.empty())
		return;

	// TEMP! Login use first character
	CClientPacket p(CMSG_PLAYER_LOGIN);
	p << characters[0].GUID;
	SendPacket(p);
}

void WoWWorld::S_SMSG_LOGIN_VERIFY_WORLD(CServerPacket& Buffer)
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
	skyManager->Load(mapID);

	// Map
	map->MapPreLoad(m_Scene.GetBaseManager().GetManager<CDBCStorage>()->DBC_Map()[mapID]);
	map->MapLoad();
	map->EnterMap(position);

	// Camera
	m_Scene.GetCameraController()->GetCamera()->SetPosition(position);
	m_Scene.GetCameraController()->GetCamera()->SetYaw(48.8);
	m_Scene.GetCameraController()->GetCamera()->SetPitch(-27.8);
}

void WoWWorld::S_SMSG_COMPRESSED_UPDATE_OBJECT(CServerPacket& Buffer)
{
	uint32 dataSize;
	Buffer >> dataSize;

	CByteBuffer uncompressed;
	uncompressed.writeDummy(dataSize);

	uLongf uncompressedSize = dataSize;
	if (uncompress(uncompressed.getDataEx(), &uncompressedSize, Buffer.getDataFromCurrent(), Buffer.getSize() - 4) != Z_OK)
	{
		Log::Error("SMSG_COMPRESSED_UPDATE_OBJECT: Error while uncompress object.");
		return;
	}
	_ASSERT(dataSize == uncompressedSize);

	uncompressed.seek(0);
	ProcessUpdatePacket(uncompressed);
}

void WoWWorld::S_SMSG_UPDATE_OBJECT(CServerPacket& Buffer)
{
	ProcessUpdatePacket(Buffer);
}

namespace
{
	enum MonsterMoveType : uint8
	{
		MonsterMoveNormal = 0,
		MonsterMoveStop = 1,
		MonsterMoveFacingSpot = 2,
		MonsterMoveFacingTarget = 3,
		MonsterMoveFacingAngle = 4
	};

#pragma pack(push, 1)
	class MoveSplineFlag
	{
	public:
		enum eFlags
		{
			None = 0x00000000,
			// x00-xFF(first byte) used as animation Ids storage in pair with Animation flag
			Done = 0x00000100,
			Falling = 0x00000200,           // Affects elevation computation, can't be combined with Parabolic flag
			No_Spline = 0x00000400,
			Parabolic = 0x00000800,           // Affects elevation computation, can't be combined with Falling flag
			CanSwim = 0x00001000,
			Flying = 0x00002000,           // Smooth movement(Catmullrom interpolation mode), flying animation
			OrientationFixed = 0x00004000,           // Model orientation fixed
			Final_Point = 0x00008000,
			Final_Target = 0x00010000,
			Final_Angle = 0x00020000,
			Catmullrom = 0x00040000,           // Used Catmullrom interpolation mode
			Cyclic = 0x00080000,           // Movement by cycled spline
			Enter_Cycle = 0x00100000,           // Everytimes appears with cyclic flag in monster move packet, erases first spline vertex after first cycle done
			Animation = 0x00200000,           // Plays animation after some time passed
			Frozen = 0x00400000,           // Will never arrive
			TransportEnter = 0x00800000,
			TransportExit = 0x01000000,
			Unknown7 = 0x02000000,
			Unknown8 = 0x04000000,
			Backward = 0x08000000,
			Unknown10 = 0x10000000,
			Unknown11 = 0x20000000,
			Unknown12 = 0x40000000,
			Unknown13 = 0x80000000,

			// Masks
			Mask_Final_Facing = Final_Point | Final_Target | Final_Angle,
			// animation ids stored here, see AnimType enum, used with Animation flag
			Mask_Animations = 0xFF,
			// flags that shouldn't be appended into SMSG_MONSTER_MOVE\SMSG_MONSTER_MOVE_TRANSPORT packet, should be more probably
			Mask_No_Monster_Move = Mask_Final_Facing | Mask_Animations | Done,
			// CatmullRom interpolation mode used
			Mask_CatmullRom = Flying | Catmullrom,
			// Unused, not suported flags
			Mask_Unused = No_Spline | Enter_Cycle | Frozen | Unknown7 | Unknown8 | Unknown10 | Unknown11 | Unknown12 | Unknown13
		};

		inline uint32& raw() { return (uint32&)*this; }
		inline uint32 const& raw() const { return (uint32 const&)*this; }

		MoveSplineFlag() { raw() = 0; }
		MoveSplineFlag(uint32 f) { raw() = f; }
		MoveSplineFlag(MoveSplineFlag const& f) { raw() = f.raw(); }

		// Constant interface

		bool isSmooth() const { return (raw() & Mask_CatmullRom) != 0; }
		bool isLinear() const { return !isSmooth(); }
		bool isFacing() const { return (raw() & Mask_Final_Facing) != 0; }

		uint8 getAnimationId() const { return animId; }
		bool hasAllFlags(uint32 f) const { return (raw() & f) == f; }
		bool hasFlag(uint32 f) const { return (raw() & f) != 0; }
		uint32 operator & (uint32 f) const { return (raw() & f); }
		uint32 operator | (uint32 f) const { return (raw() | f); }
		//std::string ToString() const;

		// Not constant interface

		void operator &= (uint32 f) { raw() &= f; }
		void operator |= (uint32 f) { raw() |= f; }

		void EnableAnimation(uint8 anim) { raw() = (raw() & ~(Mask_Animations | Falling | Parabolic)) | Animation | anim; }
		void EnableParabolic() { raw() = (raw() & ~(Mask_Animations | Falling | Animation)) | Parabolic; }
		void EnableFalling() { raw() = (raw() & ~(Mask_Animations | Parabolic | Flying | Animation)) | Falling; }
		void EnableFlying() { raw() = (raw() & ~(Falling | Catmullrom)) | Flying; }
		void EnableCatmullRom() { raw() = (raw() & ~Flying) | Catmullrom; }
		void EnableFacingPoint() { raw() = (raw() & ~Mask_Final_Facing) | Final_Point; }
		void EnableFacingAngle() { raw() = (raw() & ~Mask_Final_Facing) | Final_Angle; }
		void EnableFacingTarget() { raw() = (raw() & ~Mask_Final_Facing) | Final_Target; }
		void EnableTransportEnter() { raw() = (raw() & ~TransportExit) | TransportEnter; }
		void EnableTransportExit() { raw() = (raw() & ~TransportEnter) | TransportExit; }

		uint8 animId : 8;
		bool done : 1;
		bool falling : 1;
		bool no_spline : 1;
		bool parabolic : 1;
		bool canswim : 1;
		bool flying : 1;
		bool orientationFixed : 1;
		bool final_point : 1;
		bool final_target : 1;
		bool final_angle : 1;
		bool catmullrom : 1;
		bool cyclic : 1;
		bool enter_cycle : 1;
		bool animation : 1;
		bool frozen : 1;
		bool transportEnter : 1;
		bool transportExit : 1;
		bool unknown7 : 1;
		bool unknown8 : 1;
		bool backward : 1;
		bool unknown10 : 1;
		bool unknown11 : 1;
		bool unknown12 : 1;
		bool unknown13 : 1;
	};
#pragma pack(pop)
}

void WoWWorld::S_SMSG_MONSTER_MOVE(CServerPacket & Buffer)
{
	uint64 packedGUID;
	Buffer.ReadPackedUInt64(packedGUID);

	Buffer.seekRelative(1);

	ObjectGuid guid(packedGUID);

	//Log::Green("GUID is '%s'", GetLogNameForGuid(guid));

	float positionX;
	Buffer >> positionX;
	float positionY;
	Buffer >> positionY;
	float positionZ;
	Buffer >> positionZ;

	glm::vec3 position = fromGameToReal(glm::vec3(positionX, positionY, positionZ));

	uint32 splineID;
	Buffer >> splineID;

	MonsterMoveType monsterMoveType;
	Buffer.read(&monsterMoveType);

	switch (monsterMoveType)
	{
		case MonsterMoveType::MonsterMoveFacingSpot:
		{
			float x, y, z;
			Buffer >> x;
			Buffer >> y;
			Buffer >> z;
		}
		break;

		case MonsterMoveType::MonsterMoveFacingTarget:
		{
			uint64 targetGUID;
			Buffer >> targetGUID;
		}
		break;

		case MonsterMoveType::MonsterMoveFacingAngle:
		{
			float angle;
			Buffer >> angle;
		}
		break;
	}

	const auto& objIterator = m_Objects.find(guid);
	if (objIterator != m_Objects.end())
	{
		auto objAsUnit = std::dynamic_pointer_cast<WoWUnit>(objIterator->second);
		objAsUnit->SetLocalPosition(fromGameToReal(glm::vec3(positionX, positionY, positionZ)));
	}
	else
	{
		Log::Warn("OBJECT %s %d %d NOT FOUND.", guid.GetTypeName(), guid.GetTypeId(), guid.GetEntry());
	}

	MoveSplineFlag splineflags;
	Buffer.read(&splineflags);

	if (splineflags.animation)
	{
		uint8 animationID;
		Buffer << animationID;

		int32 effectStartTime;
		Buffer << effectStartTime;
	}

	int32 duration;
	Buffer << duration;

	if (splineflags.parabolic)
	{
		float vertical_acceleration;
		Buffer << vertical_acceleration;

		int32 effectStartTime;
		Buffer << effectStartTime;
	}

	/*
	if (monsterMoveType != MonsterMoveStop)
	{
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

		
		glm::vec3 direction = glm::normalize(NextPoint - position);

		// Yaw is the bearing of the forward vector's shadow in the xy plane.
		float yaw = atan2(direction.z, direction.x);

		// Find the vector in the xy plane 90 degrees to the right of our bearing.
		float planeRightX = glm::sin(yaw);
		float planeRightZ = -glm::cos(yaw);

		const auto& objIterator = m_Objects.find(guid);
		if (objIterator != m_Objects.end())
		{
			auto objAsUnit = std::dynamic_pointer_cast<WoWUnit>(objIterator->second);
			objAsUnit->SetLocalRotationEuler(glm::vec3(0.0f, -planeRightX, 0.0f));
		}
	}*/
}



//
// WoWWorld
//
void WoWWorld::AddHandler(Opcodes Opcode, std::function<void(CServerPacket&)> Handler)
{
	_ASSERT(Handler != nullptr);
	m_Handlers.insert(std::make_pair(Opcode, Handler));
}

void WoWWorld::SendPacket(CClientPacket & Bytes)
{
	m_Socket->SendPacket(Bytes);
}

bool WoWWorld::ProcessHandler(Opcodes Opcode, CServerPacket& Bytes)
{
	if (ProcessQueryResponse(Opcode, Bytes))
		return true;

	const auto& handler = m_Handlers.find(Opcode);
	if (handler != m_Handlers.end())
	{
		_ASSERT(handler->second != nullptr);
		(handler->second).operator()(Bytes);
		return true;
	}

	return false;
}

namespace
{
	struct ZN_API Block_UPDATETYPE_OUT_OF_RANGE_OBJECTS
	{
		void Fill(CByteBuffer& ByteBuffer)
		{
			uint32 OutOfRangeGUIDsCount;
			ByteBuffer >> (uint32)OutOfRangeGUIDsCount;
			OutOfRangeGUIDs.resize(OutOfRangeGUIDsCount);

			for (uint32 i = 0; i < OutOfRangeGUIDsCount; i++)
			{
				//ByteBuffer.seekRelative(sizeof(uint8)); // (uint8)0xFF;

				uint64 guid;
				ByteBuffer.ReadPackedUInt64(guid);
				OutOfRangeGUIDs[i] = guid;
			}
		}

		std::vector<uint64> OutOfRangeGUIDs;
	};
}

void WoWWorld::ProcessUpdatePacket(CByteBuffer& Bytes)
{
	uint32 BlocksCount;
	Bytes >> BlocksCount;

	for (uint32 i = 0u; i < BlocksCount; i++)
	{
		OBJECT_UPDATE_TYPE updateType;
		Bytes.read(&updateType);

		Log::Green("Process update type %d", updateType);

		switch (updateType)
		{
			case OBJECT_UPDATE_TYPE::UPDATETYPE_VALUES:
			{
				uint64 guidValue;
				Bytes.ReadPackedUInt64(guidValue);

				ObjectGuid guid(guidValue);

				std::shared_ptr<WoWObject> object = GetWoWObject(guid);
				object->UpdateValues(Bytes);
			}
			break;
			case OBJECT_UPDATE_TYPE::UPDATETYPE_MOVEMENT:
			{
				uint64 guidValue;
				Bytes.ReadPackedUInt64(guidValue);

				ObjectGuid guid(guidValue);

				std::shared_ptr<WoWObject> object = GetWoWObject(guid);
				object->ProcessMovementUpdate(Bytes);
			}
			break;
			case OBJECT_UPDATE_TYPE::UPDATETYPE_CREATE_OBJECT:
			case OBJECT_UPDATE_TYPE::UPDATETYPE_CREATE_OBJECT2:
			{
				uint64 guidValue;
				Bytes.ReadPackedUInt64(guidValue);
				ObjectGuid guid(guidValue);

				ObjectTypeID typeID;
				Bytes.read(&typeID);

				std::shared_ptr<WoWObject> object = GetWoWObject(guid);
				object->ProcessMovementUpdate(Bytes);
				object->UpdateValues(Bytes);
				object->AfterCreate(m_Scene);

				Log::Warn("UPDATETYPE_CREATE_OBJECT");
			}
			break;
			case OBJECT_UPDATE_TYPE::UPDATETYPE_OUT_OF_RANGE_OBJECTS:
			{
				Block_UPDATETYPE_OUT_OF_RANGE_OBJECTS block;
				block.Fill(Bytes);
				Log::Warn("UPDATETYPE_OUT_OF_RANGE_OBJECTS");
			}
			break;

			default:
				_ASSERT_EXPR(false, "Unknown update type");
		}
	}
}

std::shared_ptr<WoWObject> WoWWorld::CreateObjectByType(ObjectGuid guid, ObjectTypeID ObjectTypeID)
{
	SendQueryResponce(guid);

	switch (ObjectTypeID)
	{
	case ObjectTypeID::TYPEID_OBJECT:
		return WoWObject::Create(m_Scene, guid);

	case ObjectTypeID::TYPEID_ITEM:
		return WoWItem::Create(m_Scene, guid);

	case ObjectTypeID::TYPEID_CONTAINER:
		return WoWBag::Create(m_Scene, guid);

	case ObjectTypeID::TYPEID_UNIT:
		return WoWUnit::Create(m_Scene, guid);

	case ObjectTypeID::TYPEID_PLAYER:
		return WoWPlayer::Create(m_Scene, guid);

	case ObjectTypeID::TYPEID_GAMEOBJECT:
		return WoWGameObject::Create(m_Scene, guid);

	case ObjectTypeID::TYPEID_DYNAMICOBJECT:
		return WoWDynamicObject::Create(m_Scene, guid);

	case ObjectTypeID::TYPEID_CORPSE:
		return WoWCorpse::Create(m_Scene, guid);
	}

	_ASSERT(false);
	return nullptr;
}

std::shared_ptr<WoWObject> WoWWorld::GetWoWObject(ObjectGuid Guid)
{
	const auto& objIterator = m_Objects.find(Guid);
	if (objIterator != m_Objects.end())
		return objIterator->second;

	HighGuid highGuid = Guid.GetHigh();
	ObjectTypeID typeID = Guid.GetTypeId();

	_ASSERT(typeID != ObjectTypeID::TYPEID_OBJECT);
	std::shared_ptr<WoWObject> object = CreateObjectByType(Guid, typeID);
	m_Objects[Guid] = object;
	Log::Green("OBJECT %s %d %d created.", Guid.GetTypeName(), typeID, Guid.GetEntry());
	return object;
}



//
// Cache
//
void WoWWorld::SendQueryResponce(ObjectGuid Guid)
{
	HighGuid hiGuid = Guid.GetHigh();
	ObjectGuid::LowType enGuid = Guid.GetEntry();

	switch (hiGuid)
	{
	case HighGuid::GameObject:
	{
		if (m_CacheGameObjects.find(enGuid) != m_CacheGameObjects.end())
			return;

		CClientPacket queryInfo(CMSG_GAMEOBJECT_QUERY);
		queryInfo << enGuid;
		queryInfo << Guid;
		SendPacket(queryInfo);

		// Add to cache, to prevent next requests
		m_CacheGameObjects[enGuid] = SGameObjectQueryResult();
	}
	break;

	case HighGuid::Unit:
	{
		if (m_CacheCreatures.find(enGuid) != m_CacheCreatures.end())
			return;

		CClientPacket queryInfo(CMSG_CREATURE_QUERY);
		queryInfo << enGuid;
		queryInfo << Guid;
		SendPacket(queryInfo);

		// Add to cache, to prevent next requests
		m_CacheCreatures[enGuid] = SCreatureQueryResult();
	}
	break;
	}
}

bool WoWWorld::ProcessQueryResponse(Opcodes Opcode, CServerPacket& Bytes)
{
	switch (Opcode)
	{
	case SMSG_NAME_QUERY_RESPONSE:
	{

	}
	break;
	case SMSG_PET_NAME_QUERY_RESPONSE:
	{

	}
	break;
	case SMSG_GUILD_QUERY_RESPONSE:
	{

	}
	break;
	case SMSG_ITEM_QUERY_SINGLE_RESPONSE:
	{

	}
	break;
	case SMSG_ITEM_QUERY_MULTIPLE_RESPONSE:
	{

	}
	break;
	case SMSG_PAGE_TEXT_QUERY_RESPONSE:
	{

	}
	break;
	case SMSG_QUEST_QUERY_RESPONSE:
	{

	}
	break;
	case SMSG_GAMEOBJECT_QUERY_RESPONSE:
	{
		SGameObjectQueryResult gameObjectQueryResult;
		gameObjectQueryResult.Fill(Bytes);
		gameObjectQueryResult.Print();
		m_CacheGameObjects[gameObjectQueryResult.entryID] = gameObjectQueryResult;
		return true;
	}
	break;
	case SMSG_CREATURE_QUERY_RESPONSE:
	{
		SCreatureQueryResult creatureQueryResult;
		creatureQueryResult.Fill(Bytes);
		creatureQueryResult.Print();
		m_CacheCreatures[creatureQueryResult.entry] = creatureQueryResult;
		return true;
	}
	break;
	}

	return false;
}

#endif