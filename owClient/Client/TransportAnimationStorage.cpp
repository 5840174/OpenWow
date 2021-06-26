#include "stdafx.h"

// General
#include "TransportAnimationStorage.h"

CTransportAnimationStorage::CTransportAnimationStorage(IBaseManager & BaseManager)
	:m_BaseManager(BaseManager)
{}

CTransportAnimationStorage::~CTransportAnimationStorage()
{}

void CTransportAnimationStorage::Initialize()
{
	auto transportAnimations = m_BaseManager.GetManager<CDBCStorage>()->DBC_TransportAnimation();
	for (const auto& transportAnimationsIt : transportAnimations)
	{
		STransportAnimation transportAmination;
		transportAmination.Time = transportAnimationsIt->Get_TimeIndex();
		transportAmination.Position = glm::vec3(transportAnimationsIt->Get_PositionX(), transportAnimationsIt->Get_PositionY(), transportAnimationsIt->Get_PositionZ());

		m_TransportRecords[transportAnimationsIt->Get_TransportID()].push_back(transportAmination);
	}

	Log::Print("CTransportAnimationStorage::Initialize: Loaded records for '%d' transport entries.", m_TransportRecords.size());
}

const std::vector<CTransportAnimationStorage::STransportAnimation>& CTransportAnimationStorage::GetTransportAnimations(uint32 Entry) const
{
	const auto& transportRecordIt = m_TransportRecords.find(Entry);
	if (transportRecordIt == m_TransportRecords.end())
		return m_EmptyAnimations;
	return transportRecordIt->second;
}
