#include "stdafx.h"

// General
#include "TaxiStorage.h"

CowClient_DBCTaxi::CowClient_DBCTaxi(const IBaseManager& BaseManager)
	: m_BaseManager(BaseManager)
{
	auto dbcTaxiPathNodes = m_BaseManager.GetManager<CDBCStorage>()->DBC_TaxiPathNode();
	for (const auto& dbcTaxiPathNodesIt : dbcTaxiPathNodes)
	{
		STaxiPathNode pathNode;
		pathNode.MapID = dbcTaxiPathNodesIt->Get_MapID();
		pathNode.Position = glm::vec3(dbcTaxiPathNodesIt->Get_X(), dbcTaxiPathNodesIt->Get_Y(), dbcTaxiPathNodesIt->Get_Z());
		pathNode.Delay = dbcTaxiPathNodesIt->Get_Flags() == 2 ? dbcTaxiPathNodesIt->Get_Delay() : 0;
		m_PathNodes[dbcTaxiPathNodesIt->Get_TaxiPathID()].push_back(pathNode);
	}
}

CowClient_DBCTaxi::~CowClient_DBCTaxi()
{}

const std::vector<CowClient_DBCTaxi::STaxiPathNode>& CowClient_DBCTaxi::GetPathNodes(uint32 PathID) const
{
	const auto& pathNodesIt = m_PathNodes.find(PathID);
	if (pathNodesIt == m_PathNodes.end())
		return m_EmptyPathNodes;
	return pathNodesIt->second;
}
