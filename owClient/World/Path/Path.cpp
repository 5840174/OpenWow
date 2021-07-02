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

	size_t index = 0;
	for (size_t i = m_Nodes.size() - 2; i >= 0; i--)
	{
		if (GetCurrTime() >= m_Nodes.at(i).Time)
		{
			index = i;
			break;
		}
	}

	auto currNode = m_Nodes.at(index);
	auto nextNode = m_Nodes.at(index + 1);

	int32 timeLength = nextNode.Time - currNode.Time;
	if (timeLength == 0.0f)
		return currNode.Node->GetPosition();

	float positionForInterpolation = float(GetCurrTime() - currNode.Time) / float(timeLength);
	if (positionForInterpolation < 0.0f || positionForInterpolation > 1.0f)
		throw CException("Incorrect interpolationValue");

	return interpolateLinear(positionForInterpolation, currNode.Node->GetPosition(), nextNode.Node->GetPosition());
}

glm::vec3 CWoWPath::GetNextNodePosition() const
{
	if (m_Nodes.size() == 1)
		return m_Nodes.begin()->Node->GetPosition();

	size_t index = 0;
	for (size_t i = m_Nodes.size() - 2; i >= 0; i--)
	{
		if (GetCurrTime() >= m_Nodes.at(i).Time)
		{
			index = i;
			break;
		}
	}

	auto nextNode = m_Nodes.at(index + 1);
	return nextNode.Node->GetPosition();
}

void CWoWPath::CalculateTimes()
{
	if (m_Nodes.empty())
		return;

	m_FullPathLength = 0.0f;

	auto& firstPathNode = (*m_Nodes.begin());
	firstPathNode.DistanceFromPreviousPoint = 0.0f;

	glm::vec3 lastPosition = firstPathNode.Node->GetPosition();

	for (size_t i = 1; i < m_Nodes.size(); i++)
	{
		auto& currentPathNode = m_Nodes.at(i);
		glm::vec3 currentNodePosition = currentPathNode.Node->GetPosition();

		float distance = glm::distance(currentNodePosition, lastPosition);
		m_FullPathLength += distance;
		currentPathNode.DistanceFromPreviousPoint = distance;

		lastPosition = currentNodePosition;
	}

	float distanceSumma = 0.0f;
	for (size_t i = 0; i < m_Nodes.size(); i++)
	{
		auto& currentPathNode = m_Nodes.at(i);

		distanceSumma += currentPathNode.DistanceFromPreviousPoint;

		float distanceKoeff = distanceSumma / m_FullPathLength;
		int32 calculatedTime = float(m_FullPathDuration) * distanceKoeff;
		currentPathNode.Time = calculatedTime;
	}

	auto nodesIt = m_Nodes.begin();
	while (nodesIt != m_Nodes.end())
	{
		if (nodesIt == m_Nodes.begin())
		{
			nodesIt++;
			continue;
		}

		if (nodesIt->DistanceFromPreviousPoint == 0.0f)
		{
			nodesIt = m_Nodes.erase(nodesIt);
			continue;
		}

		nodesIt++;
	}
}
