#include "stdafx.h"

// General
#include "Path.h"

CWoWPath::CWoWPath()
	: m_FullPathDuration(0)
	, m_FullPathLength(0.0f)
	, m_CurrTime(0)
{}

CWoWPath::~CWoWPath()
{}

std::vector<std::shared_ptr<CWoWPathNode>> CWoWPath::GetPathNodes() const
{
	std::vector<std::shared_ptr<CWoWPathNode>> pathNodes;
	std::for_each(m_Nodes.begin(), m_Nodes.end(), [&pathNodes](const SPathNode& PathNode) {
		pathNodes.push_back(PathNode.Node);
	});
	return pathNodes;
}

size_t CWoWPath::GetPathNodesCount() const
{
	return m_Nodes.size();
}

std::shared_ptr<CWoWPathNode> CWoWPath::GetPathNode(size_t Index) const
{
	if (Index >= m_Nodes.size())
		throw CException("CWoWPath::GetPath: Index '%d' is out of bounds.", Index);
	return m_Nodes.at(Index).Node;
}

void CWoWPath::AddPathNode(std::shared_ptr<CWoWPathNode> PathNode)
{
	m_Nodes.push_back(SPathNode(PathNode));
	CalculateTimes();
}

int32 CWoWPath::GetDuration() const
{
	return m_FullPathDuration;
}

void CWoWPath::SetDuration(int32 Duration)
{
	m_FullPathDuration = Duration;
	CalculateTimes();
}

int32 CWoWPath::GetCurrTime() const
{
	return m_CurrTime;
}

void CWoWPath::SetCurrTime(int32 CurrTime)
{
	if (CurrTime > m_FullPathDuration)
		throw CException("Current time must be less then duration. (%d, %d).", CurrTime, m_FullPathDuration);
	m_CurrTime = CurrTime;
}

void CWoWPath::AddCurrTime(int32 Value)
{
	if (m_CurrTime + Value <= m_FullPathDuration)
		m_CurrTime += Value;
}

glm::vec3 CWoWPath::GetPositionByCurrTime() const
{
	if (m_Nodes.size() == 1)
		return m_Nodes.begin()->Node->GetPosition();

	size_t currentNodeIndex = GetCurrentNodeIndex();

	auto currNode = m_Nodes.at(currentNodeIndex);
	uint32 timeBeginsFromCurrentPoint = GetCurrTime() - currNode.Timestamp;

	// Unit waits on point
	if (timeBeginsFromCurrentPoint < currNode.Node->GetTimeDelay())
		return currNode.Node->GetPosition();

	auto nextNode = m_Nodes.at(currentNodeIndex + 1);
	int32 timeLength = nextNode.Timestamp - currNode.Timestamp;
	if (timeLength == 0.0f)
		return currNode.Node->GetPosition();

	float positionForInterpolation = float(timeBeginsFromCurrentPoint - currNode.Node->GetTimeDelay()) / float(timeLength);
	if (positionForInterpolation < 0.0f || positionForInterpolation > 1.0f)
		throw CException("Incorrect interpolationValue");

	return interpolateLinear(positionForInterpolation, currNode.Node->GetPosition(), nextNode.Node->GetPosition());
}

glm::vec3 CWoWPath::GetNextNodePosition() const
{
	if (m_Nodes.size() == 1)
		return m_Nodes.begin()->Node->GetPosition();

	size_t currentNodeIndex = GetCurrentNodeIndex();

	auto nextNode = m_Nodes.at(currentNodeIndex + 1);
	return nextNode.Node->GetPosition();
}

void CWoWPath::CalculateTimes()
{
	if (m_Nodes.empty())
		return;

	m_FullPathLength = 0.0f;

	auto& firstNode = (*m_Nodes.begin());
	glm::vec3 previousNodePosition = firstNode.Node->GetPosition();

	for (auto& nodesIt : m_Nodes)
	{
		glm::vec3 currentNodePosition = nodesIt.Node->GetPosition();

		float distance = glm::distance(currentNodePosition, previousNodePosition);
		m_FullPathLength += distance;
		nodesIt.DistanceToPreviousPoint = distance;

		previousNodePosition = currentNodePosition;
	}

	uint32 timeWithoutStopOnNodes = m_FullPathDuration;
	for (const auto& nodesIt : m_Nodes)
		timeWithoutStopOnNodes -= nodesIt.Node->GetTimeDelay();
	if (timeWithoutStopOnNodes < 0)
		throw CException("FullPathDuration must include all nodes TimeDelays!");

	float distanceSumma = 0.0f;
	uint32 timeSumma = 0;
	for (auto& nodesIt : m_Nodes)
	{
		distanceSumma += nodesIt.DistanceToPreviousPoint;
		
		float distanceKoeff = distanceSumma / m_FullPathLength;
		int32 calculatedTime = float(timeWithoutStopOnNodes) * distanceKoeff;
		nodesIt.Timestamp = calculatedTime + timeSumma;

		timeSumma += nodesIt.Node->GetTimeDelay();
	}

	/*auto nodesIt = m_Nodes.begin();
	while (nodesIt != m_Nodes.end())
	{
		if (nodesIt == m_Nodes.begin())
		{
			nodesIt++;
			continue;
		}

		if (nodesIt->DistanceToPreviousPoint == 0.0f && nodesIt)
		{
			nodesIt = m_Nodes.erase(nodesIt);
			continue;
		}

		nodesIt++;
	}*/
}

size_t CWoWPath::GetCurrentNodeIndex() const
{
	if (m_Nodes.size() == 1)
		return 0;

	size_t findedIndex = 0;
	for (size_t i = 0; i < m_Nodes.size() - 1; i++)
	{
		const auto& node = m_Nodes.at(i);
		if (node.Timestamp <= GetCurrTime())
			findedIndex = i;
	}

	return findedIndex;
}
