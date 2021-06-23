#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "World.h"

// Additional
#include "Client/Templates/CharacterTemplate.h"





CWoWWorld::CWoWWorld(IScene& Scene, const std::shared_ptr<CWorldSocket>& Socket)
	: m_Scene(Scene)
	, m_Socket(Socket)
	, m_WorldObjects(*this, m_Scene)
	, m_WorldObjectUpdater(*this, m_Scene)
	, m_ClientCache(*this)
	, m_TaxiStorage(m_Scene.GetBaseManager())
	, m_TransportAnimationStorage(m_Scene.GetBaseManager())
{
	skyManager = Scene.GetRootSceneNode()->CreateSceneNode<SkyManager>();
	Scene.GetBaseManager().AddManager<ISkyManager>(skyManager);

	map = Scene.GetRootSceneNode()->CreateSceneNode<CMap>();

	m_TaxiStorage.Initialize();
	m_TransportAnimationStorage.Initialize();

	// Handlers
	AddHandler(SMSG_LOGIN_VERIFY_WORLD, std::bind(&CWoWWorld::S_SMSG_LOGIN_VERIFY_WORLD, this, std::placeholders::_1));
	AddHandler(SMSG_TIME_SYNC_REQ, std::bind(&CWoWWorld::On_SMSG_TIME_SYNC_REQ, this, std::placeholders::_1));
	AddHandler(SMSG_MONSTER_MOVE, std::bind(&CWoWWorld::S_SMSG_MONSTER_MOVE, this, std::placeholders::_1));
	AddHandler(SMSG_DESTROY_OBJECT, std::bind(&CWoWWorld::S_SMSG_DESTROY_OBJECT, this, std::placeholders::_1));
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
	skyManager->Load(mapID);

	// Map
	map->MapPreLoad(m_Scene.GetBaseManager().GetManager<CDBCStorage>()->DBC_Map()[mapID]);
	map->MapLoad();
	map->EnterMap(position);

	// Camera
	m_Scene.GetCameraController()->GetCamera()->SetPosition(position);
	m_Scene.GetCameraController()->GetCamera()->SetYaw(glm::degrees(orientation));
	m_Scene.GetCameraController()->GetCamera()->SetPitch(0.0f);
}

void CWoWWorld::On_SMSG_TIME_SYNC_REQ(CServerPacket & Buffer)
{
	uint32 timeSyncNextCounter;
	Buffer >> timeSyncNextCounter;

	Log::Info("On_SMSG_TIME_SYNC_REQ: timeSyncNextCounter = '%d'.", timeSyncNextCounter);
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
}

void CWoWWorld::S_SMSG_MONSTER_MOVE(CServerPacket & Buffer)
{
	uint64 packedGUID;
	Buffer.ReadPackedUInt64(packedGUID);

	Buffer.seekRelative(1);

	CWoWObjectGuid guid(packedGUID);
	if (guid.GetTypeId() != EWoWObjectTypeID::TYPEID_UNIT)
		throw CException("CWoWWorld::S_SMSG_MONSTER_MOVE: Movement packet accept only EWoWObjectTypeID::TYPEID_UNIT.");

	

	float positionX;
	Buffer >> positionX;
	float positionY;
	Buffer >> positionY;
	float positionZ;
	Buffer >> positionZ;

	glm::vec3 position = fromGameToReal(glm::vec3(positionX, positionY, positionZ));

	if (auto object = m_WorldObjects.GetWoWObject(guid))
	{
		auto objectAsUnit = std::dynamic_pointer_cast<WoWUnit>(object);
		objectAsUnit->SetLocalPosition(fromGameToReal(glm::vec3(positionX, positionY, positionZ)));
	}

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

		case MonsterMoveType::MonsterMoveNormal:
		{
			// normal packet
		}
		break;

		case MonsterMoveType::MonsterMoveStop:
			return;
	}

	Movement::MoveSplineFlag splineflags;
	Buffer.read(&splineflags);

	if (splineflags.animation)
	{
		uint8 animationID;
		Buffer >> animationID;

		int32 effectStartTime;
		Buffer >> effectStartTime;
	}

	int32 duration;
	Buffer >> duration;

	if (splineflags.parabolic)
	{
		float vertical_acceleration;
		Buffer >> vertical_acceleration;

		int32 effectStartTime;
		Buffer >> effectStartTime;
	}


	if (splineflags & Movement::MoveSplineFlag::Mask_CatmullRom)
	{
		uint32 pathPointsCnt;
		Buffer >> pathPointsCnt;

		for (uint32 i = 0u; i < pathPointsCnt; i++)
		{
			float positionX1;
			Buffer >> positionX1;
			float positionY1;
			Buffer >> positionY1;
			float positionZ1;
			Buffer >> positionZ1;
		}

		//(splineflags.cyclic)
	}
	else // linear
	{
		uint32 count;
		Buffer >> count;

		float lastPointX, lastPointY, lastPointZ;
		Buffer >> lastPointX;
		Buffer >> lastPointY;
		Buffer >> lastPointZ;

		if (auto object = m_WorldObjects.GetWoWObject(guid))
		{
			auto objectAsUnit = std::dynamic_pointer_cast<WoWUnit>(object);
			objectAsUnit->DestinationPoint = fromGameToReal(glm::vec3(lastPointX, lastPointY, lastPointZ));
		}

		if (count > 1)
		{
			for (uint32 i = 1; i < count; ++i)
			{
				uint32 packedPos;
				Buffer >> packedPos;

				//G3D::Vector3 middle = (real_path[0] + real_path[last_idx]) / 2.f;
				//G3D::Vector3 offset;
				// first and last points already appended
				//for (uint32 i = 1; i < last_idx; ++i)
				//{
				//	offset = middle - real_path[i];

				// this is offsets to path
				float x2 = ((packedPos      ) & 0x7FF) * 0.25;
				float y2 = ((packedPos >> 11) & 0x7FF) * 0.25;
				float z2 = ((packedPos >> 22) & 0x3FF) * 0.25;
			}
		}
	}


	/*
	 uint32 packed = 0;
            packed |= ((int)(x / 0.25f) & 0x7FF);
            packed |= ((int)(y / 0.25f) & 0x7FF) << 11;
            packed |= ((int)(z / 0.25f) & 0x3FF) << 22;
            *this << packed;
	*/

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

		const auto& objIterator = m_WoWObjects.find(guid);
		if (objIterator != m_WoWObjects.end())
		{
			auto objAsUnit = std::dynamic_pointer_cast<WoWUnit>(objIterator->second);
			objAsUnit->SetLocalRotationEuler(glm::vec3(0.0f, -planeRightX, 0.0f));
		}
	}*/
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

bool CWoWWorld::ProcessHandler(Opcodes Opcode, CServerPacket& Bytes)
{
	const auto& handler = m_Handlers.find(Opcode);
	if (handler != m_Handlers.end())
	{
		_ASSERT(handler->second != nullptr);
		(handler->second).operator()(Bytes);

		if (Bytes.getPos() != Bytes.getSize())
			throw CException("CWoWWorld::ProcessHandler: Packet '%d' is not fully readed. %d of %d.", Opcode, Bytes.getPos(), Bytes.getSize());

		return true;
	}

	return false;
}

void CWoWWorld::SendPacket(CClientPacket & Bytes)
{
	m_Socket->SendPacket(Bytes);
}




#endif