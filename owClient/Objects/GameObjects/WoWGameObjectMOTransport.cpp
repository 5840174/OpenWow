#include "stdafx.h"

// General
#include "WoWGameObjectMOTransport.h"

// Additional
#include "../../World/ServerWorld.h"

#include "Client/TaxiStorage.h"

CowServerGameObject_MOTransport::CowServerGameObject_MOTransport(IScene & Scene, CowServerWorld & WoWWorld, CowGuid Guid)
	: CowServerGameObject(Scene, WoWWorld, Guid)
	, m_PathID(0)
	, m_PathProgress(0.0f)
{}

CowServerGameObject_MOTransport::~CowServerGameObject_MOTransport()
{}

uint32 CowServerGameObject_MOTransport::GetPathID() const
{
	return m_PathID;
}

void CowServerGameObject_MOTransport::OnValueUpdated(uint16 index)
{
	__super::OnValueUpdated(index);

	if (index == GAMEOBJECT_DYNAMIC)
	{
		uint16 pathProgress = m_Values.GetUInt16Value(GAMEOBJECT_DYNAMIC, 1);
		m_PathProgress = float(pathProgress) / 65535.0f;		
	}

	if (index == GAMEOBJECT_LEVEL)
	{
		if (m_WoWPath == nullptr)
			m_WoWPath = MakeShared(CWoWPath);
		m_WoWPath->SetDuration(m_Values.GetUInt32Value(GAMEOBJECT_LEVEL));
		m_WoWPath->SetCurrTime(m_PathProgress * float(m_WoWPath->GetDuration()));
	}
}

void CowServerGameObject_MOTransport::OnTemplate(CowGuid::EntryType_t Entry, const std::shared_ptr<SGameObjectQueryResult>& QueryResult)
{
	__super::OnTemplate(Entry, QueryResult);

	m_TransportName = QueryResult->Name;
	m_PathID = QueryResult->moTransport.taxiPathId;

	const auto& taxiNodes = GetWoWWorld().GetClientCache().GetTaxiStorage().GetPathNodes(m_PathID);

	if (m_WoWPath == nullptr)
		m_WoWPath = MakeShared(CWoWPath);


	for (auto it = taxiNodes.begin(); it != taxiNodes.end(); it++)
	{
		//if (it->MapID != GetWoWWorld().GetMap()->GetMapID())
		//	continue;

		auto pathNode = MakeShared(CWoWPathNode, fromGameToReal(it->Position));
		pathNode->SetMapID(it->MapID);
		pathNode->SetTimeDelay(it->Delay * 1000);
		m_WoWPath->AddPathNode(pathNode);
	}	
}



//
// ISceneNode
//
void CowServerGameObject_MOTransport::Update(const UpdateEventArgs & e)
{
	__super::Update(e);


	if (m_WoWPath)
	{
		m_WoWPath->AddCurrTime(e.DeltaTime);

		if (m_WoWPath->GetPathNodesCount() > 0)
		{
			glm::vec3 NextPoint = m_WoWPath->GetPositionByCurrTime();
			if (glm::distance(NextPoint, Position) > 0.01f)
			{
				glm::vec3 test = m_WoWPath->GetNextNodePosition();

				glm::vec3 directionVec = glm::normalize(glm::vec3(test.x, 0.0f, test.z) - glm::vec3(Position.x, 0.0f, Position.z));
				float yaw = atan2(directionVec.x, directionVec.z);

				Position = NextPoint;
				Orientation = glm::degrees(yaw + glm::half_pi<float>());
				CommitPositionAndRotation();
			}
		}
	}

#if 0

	GameobjectTypes type = GameobjectTypes(GetByteValue(GAMEOBJECT_BYTES_1, 1));
	if (type != GameobjectTypes::GAMEOBJECT_TYPE_MO_TRANSPORT)
		throw CException("CowServerGameObject_MOTransport::Update: Object type must be GAMEOBJECT_TYPE_MO_TRANSPORT.");

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

std::shared_ptr<CowServerGameObject_MOTransport> CowServerGameObject_MOTransport::Create(CowServerWorld & WoWWorld, IScene & Scene, CowGuid Guid)
{
	std::shared_ptr<CowServerGameObject_MOTransport> thisObj = MakeShared(CowServerGameObject_MOTransport, Scene, WoWWorld, Guid);
	//Log::Green("---Create CowServerGameObject_MOTransport with Type '%s', Entry '%d', Counter '%d'", Guid.GetTypeName(), Guid.GetEntry(), Guid.GetCounter());
	return thisObj;
}

void CowServerGameObject_MOTransport::Destroy()
{
	__super::Destroy();
}
