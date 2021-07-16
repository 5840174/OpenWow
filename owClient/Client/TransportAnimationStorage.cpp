#include "stdafx.h"

// General
#include "TransportAnimationStorage.h"

CowClient_DBCTransport::CowClient_DBCTransport(IBaseManager & BaseManager)
	:m_BaseManager(BaseManager)
{}

CowClient_DBCTransport::~CowClient_DBCTransport()
{}

void CowClient_DBCTransport::Initialize()
{
	auto transportAnimations = m_BaseManager.GetManager<CDBCStorage>()->DBC_TransportAnimation();
	for (const auto& transportAnimationsIt : transportAnimations)
	{
		STransportAnimation transportAmination;
		transportAmination.Time = transportAnimationsIt->Get_TimeIndex();
		transportAmination.Position = glm::vec3(transportAnimationsIt->Get_PositionX(), transportAnimationsIt->Get_PositionY(), transportAnimationsIt->Get_PositionZ());

		m_TransportRecords[transportAnimationsIt->Get_TransportID()].push_back(transportAmination);
	}

	Log::Print("CowClient_DBCTransport::Initialize: Loaded records for '%d' transport entries.", m_TransportRecords.size());
}

const std::vector<CowClient_DBCTransport::STransportAnimation>& CowClient_DBCTransport::GetTransportAnimations(uint32 Entry) const
{
	const auto& transportRecordIt = m_TransportRecords.find(Entry);
	if (transportRecordIt == m_TransportRecords.end())
		return m_EmptyAnimations;
	return transportRecordIt->second;
}
