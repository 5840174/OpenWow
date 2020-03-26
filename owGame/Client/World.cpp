#include "stdafx.h"

// General
#include "World.h"

// Additional
#include "Client/Templates/CharacterTemplate.h"

// Types
#include "WoWBag.h"
#include "WoWGameObject.h"
#include "WoWItem.h"
#include "WoWPlayer.h"
#include "WoWUnit.h"

// zlib
#include "zlib/zlib.h"
#pragma comment(lib, "zlib.lib")

WoWWorld::WoWWorld(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene* Scene, const std::shared_ptr<CWorldSocket>& Socket)
	: m_Scene(Scene)
	, m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
	, m_Socket(Socket)
{
	m_Socket->SetExternalHandler(std::bind(&WoWWorld::ProcessHandler, this, std::placeholders::_1, std::placeholders::_2));

	skyManager = Scene->GetRootNode3D()->CreateSceneNode<SkyManager>(m_RenderDevice);
	map = Scene->GetRootNode3D()->CreateSceneNode<CMap>(m_BaseManager, m_RenderDevice);

	// Handlers
	AddHandler(SMSG_CHAR_ENUM, std::bind(&WoWWorld::S_SMSG_CHAR_ENUM, this, std::placeholders::_1));
	AddHandler(SMSG_LOGIN_VERIFY_WORLD, std::bind(&WoWWorld::S_SMSG_LOGIN_VERIFY_WORLD, this, std::placeholders::_1));
	AddHandler(SMSG_COMPRESSED_UPDATE_OBJECT, std::bind(&WoWWorld::S_SMSG_COMPRESSED_UPDATE_OBJECT, this, std::placeholders::_1));
	AddHandler(SMSG_UPDATE_OBJECT, std::bind(&WoWWorld::S_SMSG_UPDATE_OBJECT, this, std::placeholders::_1));
	//AddHandler(SMSG_MONSTER_MOVE, std::bind(&WoWWorld::S_SMSG_MONSTER_MOVE, this, std::placeholders::_1));
}

WoWWorld::~WoWWorld()
{
	while (m_Socket->ErasedByHandler() == false)
	{
		Log::Info("Wait for close socket...");
		Sleep(10);
	}
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

	// Skies
	skyManager->Load(mapID);

	// Map
	map->MapPreLoad(m_BaseManager.GetManager<CDBCStorage>()->DBC_Map()[mapID]);
	map->MapLoad();
	map->EnterMap(position);

	// Camera
	m_Scene->GetCameraController()->GetCamera()->SetTranslation(position);
	m_Scene->GetCameraController()->GetCamera()->SetYaw(48.8);
	m_Scene->GetCameraController()->GetCamera()->SetPitch(-27.8);
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

void WoWWorld::S_SMSG_MONSTER_MOVE(CServerPacket & Buffer)
{
	ObjectGuid guid;
	Buffer >> guid;

	//Log::Green("GUID is '%s'", GetLogNameForGuid(guid));

	float positionX;
	Buffer >> positionX;
	float positionY;
	Buffer >> positionY;
	float positionZ;
	Buffer >> positionZ;

	glm::vec3 position = fromGameToReal(glm::vec3(positionX, positionY, positionZ));

	uint32 msTime;
	Buffer >> msTime;

	uint8 isStopped;
	Buffer >> isStopped;


	const auto& objIterator = m_Objects.find(guid);
	if (objIterator != m_Objects.end())
	{
		auto objAsUnit = std::dynamic_pointer_cast<WoWUnit>(objIterator->second);
		objAsUnit->SetTranslate(fromGameToReal(glm::vec3(positionX, positionY, positionZ)));
	}

	if (isStopped == 0)
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
			break;
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
			objAsUnit->SetRotation(glm::vec3(0.0f, -planeRightX, 0.0f));
		}
	}
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
			object->AfterCreate(m_BaseManager, m_RenderDevice, m_Scene);

			Log::Warn("UPDATETYPE_CREATE_OBJECT");
		}
		break;
		/*case OBJECT_UPDATE_TYPE::UPDATETYPE_CREATE_OBJECT2:
		{
			Bytes.seekRelative(1); // 0xFF
			uint64 guid;
			Bytes >> guid;

			ObjectTypeID typeID;
			Bytes.read(&typeID);
		}
		break;*/
		case OBJECT_UPDATE_TYPE::UPDATETYPE_OUT_OF_RANGE_OBJECTS:
		{
			Block_UPDATETYPE_OUT_OF_RANGE_OBJECTS block;
			block.Fill(Bytes);
			Log::Warn("UPDATETYPE_OUT_OF_RANGE_OBJECTS");
		}
		break;
		default:
			Log::Error("Unknown update type %d", updateType);
			_ASSERT(false);
		}
	}
}

std::shared_ptr<WoWObject> WoWWorld::CreateObjectByType(ObjectGuid guid, ObjectTypeID ObjectTypeID)
{
	SendQueryResponce(guid);

	switch (ObjectTypeID)
	{
	case ObjectTypeID::TYPEID_OBJECT:
		return WoWObject::Create(m_BaseManager, m_RenderDevice, m_Scene, guid);

	case ObjectTypeID::TYPEID_ITEM:
		return WoWItem::Create(m_BaseManager, m_RenderDevice, m_Scene, guid);

	case ObjectTypeID::TYPEID_CONTAINER:
		return WoWBag::Create(m_BaseManager, m_RenderDevice, m_Scene, guid);

	case ObjectTypeID::TYPEID_UNIT:
		return WoWUnit::Create(m_BaseManager, m_RenderDevice, m_Scene, guid);

	case ObjectTypeID::TYPEID_PLAYER:
		return WoWPlayer::Create(m_BaseManager, m_RenderDevice, m_Scene, guid);

	case ObjectTypeID::TYPEID_GAMEOBJECT:
		return WoWGameObject::Create(m_BaseManager, m_RenderDevice, m_Scene, guid);

	case ObjectTypeID::TYPEID_DYNAMICOBJECT:
		_ASSERT(false);
		return nullptr;

	case ObjectTypeID::TYPEID_CORPSE:
		_ASSERT(false);
		return nullptr;
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