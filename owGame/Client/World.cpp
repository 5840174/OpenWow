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

void CWoWWorld::Update(const UpdateEventArgs & e)
{
	m_WorldObjects.Update(e);
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
	if (auto lockedSocket = m_Socket.lock())
		lockedSocket->SendPacket(Bytes);
}




#endif