#include "stdafx.h"

// General
#include "WorldClient.h"

// Additional
#include "WMO/WMO_Base_Instance.h"
#include "M2/M2_Base_Instance.h"

CWorldClient::CWorldClient(const IBaseManager& BaseManager, IScene& Scene)
	: m_BaseManager(BaseManager)
	, m_Scene(Scene)
{}

CWorldClient::~CWorldClient()
{}

void CWorldClient::Initialize()
{
	m_Creator = MakeShared(CWorldObjectCreator, *this, GetBaseManager());
	m_Sky = m_Scene.GetRootSceneNode()->CreateSceneNode<CSkyManager>();
	m_Map = m_Scene.GetRootSceneNode()->CreateSceneNode<CMap>(*this);
}

void CWorldClient::Update(const UpdateEventArgs & e)
{
	CMapWMOInstance::reset();
	CMapM2Instance::reset();

	m_Sky->Update(e);
	m_Map->Update(e);
}

void CWorldClient::EnterWorld(uint32 MapID, int32 TileX, int32 TileZ)
{
	m_Sky->Load(MapID);

	m_Map->MapPreLoad(GetBaseManager().GetManager<CDBCStorage>()->DBC_Map()[MapID]);
	m_Map->MapLoad();
	m_Map->EnterMap(TileX, TileZ);
}

void CWorldClient::EnterWorld(uint32 MapID, glm::vec3 Position)
{
	m_Sky->Load(MapID);

	m_Map->MapPreLoad(GetBaseManager().GetManager<CDBCStorage>()->DBC_Map()[MapID]);
	m_Map->MapLoad();
	m_Map->EnterMap(Position);
}
