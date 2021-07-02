#include "stdafx.h"

// General
#include "WoWGameObjectMOTransport.h"

// Additional
#include "../World.h"

#include "Client/TaxiStorage.h"

WoWGameObjectMOTransport::WoWGameObjectMOTransport(IScene & Scene, CWoWWorld & WoWWorld, CWoWGuid Guid)
	: WoWGameObject(Scene, WoWWorld, Guid)
	, m_PathID(0)
	, m_PathProgress(0.0f)
{}

WoWGameObjectMOTransport::~WoWGameObjectMOTransport()
{}

uint32 WoWGameObjectMOTransport::GetPathID() const
{
	return m_PathID;
}

void WoWGameObjectMOTransport::OnValueUpdated(uint16 index)
{
	__super::OnValueUpdated(index);

	if (index == GAMEOBJECT_DYNAMIC)
	{
		uint16 pathProgress = m_Values.GetUInt16Value(GAMEOBJECT_DYNAMIC, 1);
		m_PathProgress = float(pathProgress) / 65535.0f;
	}
}

void WoWGameObjectMOTransport::OnTemplate(CWoWGuid::EntryType_t Entry, const std::shared_ptr<SGameObjectQueryResult>& QueryResult)
{
	__super::OnTemplate(Entry, QueryResult);

	m_TransportName = QueryResult->Name;
	m_PathID = QueryResult->moTransport.taxiPathId;

	const auto& taxiNodes = GetWoWWorld().GetTaxiStorage().GetPathNodes(m_PathID);

	auto path = MakeShared(CWoWPath);
	for (const auto& taxiNodesIt : taxiNodes)
	{
		auto pathNode = MakeShared(CWoWPathNode, fromGameToReal(taxiNodesIt.Position));
		pathNode->SetMapID(taxiNodesIt.MapID);
		pathNode->SetTimeDelay(taxiNodesIt.Delay * 1000);
		path->AddPathNode(pathNode);
	}
	m_WoWPath = path;
}



//
// ISceneNode
//
void WoWGameObjectMOTransport::Update(const UpdateEventArgs & e)
{
	__super::Update(e);

#if 0

	GameobjectTypes type = GameobjectTypes(GetByteValue(GAMEOBJECT_BYTES_1, 1));
	if (type != GameobjectTypes::GAMEOBJECT_TYPE_MO_TRANSPORT)
		throw CException("WoWGameObjectMOTransport::Update: Object type must be GAMEOBJECT_TYPE_MO_TRANSPORT.");

	if (m_GameObjectTemplate == nullptr)
		return;

	m_PathID = m_GameObjectTemplate->moTransport.taxiPathId;
	m_TransportName   = m_GameObjectTemplate->Name;

	const auto& taxiNodes = GetWoWWorld().GetTaxiStorage().GetPathNodes(m_PathID);

	uint16 pathProgress = GetUInt16Value(GAMEOBJECT_DYNAMIC, 1);
	float pathProgressFloat = float(pathProgress) / 65535.0f;

	//m_PathProgress += e.DeltaTime / 10000.0f;
	//if (m_PathProgress > 1.0f)
	//	m_PathProgress = 0.0f;

	//if (pathProgressFloat > 1.0f)
	//	throw CException("Path progress bigger then 1.0f.");

	size_t roundedProgress = glm::ceil(static_cast<float>(taxiNodes.size()) * m_PathProgress);
	if (roundedProgress < 0 || roundedProgress >= taxiNodes.size())
		roundedProgress = 0;

	const auto& selectedNode = taxiNodes[roundedProgress];

	auto newPos = fromGameToReal(selectedNode.Position);
	if (Position != newPos)
	{
		//Log::Error("Position '%s' from '(%.1f %.1f %.1f)' MapID '%d'. TO '(%.1f, %.1f, %.1f) MapID '%d'.'", m_TransportName.c_str(), Position.x, Position.y, Position.z, MapID, newPos.x, newPos.y, newPos.z, selectedNode.MapID);
		
		//MapID = selectedNode.MapID;
		//Position = newPos;
		//CommitPositionAndRotation();
	}

#endif
	//Log::Print("GAMEOBJECT_DYNAMIC: %s - (uint %d) (float %f) Nodes %d", m_GameObjectTemplate->Name.c_str(), pathProgress, pathProgressFloat, taxiNodes.size());
}

std::shared_ptr<WoWGameObjectMOTransport> WoWGameObjectMOTransport::Create(CWoWWorld & WoWWorld, IScene & Scene, CWoWGuid Guid)
{
	std::shared_ptr<WoWGameObjectMOTransport> thisObj = MakeShared(WoWGameObjectMOTransport, Scene, WoWWorld, Guid);
	Log::Green("---Create WoWGameObjectMOTransport with Type '%s', Entry '%d', Counter '%d'", Guid.GetTypeName(), Guid.GetEntry(), Guid.GetCounter());
	return thisObj;
}

void WoWGameObjectMOTransport::Destroy()
{
	__super::Destroy();
}
