#include "stdafx.h"

// General
#include "PathNode.h"

CWoWPathNode::CWoWPathNode()
	: m_Position(glm::vec3(0.0f))
	, m_MapID(-1)
{}

CWoWPathNode::CWoWPathNode(const glm::vec3& Point)
	: m_Position(Point)
	, m_MapID(-1)
{}

CWoWPathNode::~CWoWPathNode()
{}

const glm::vec3& CWoWPathNode::GetPosition() const
{
	return m_Position;
}

void CWoWPathNode::SetPosition(const glm::vec3 & Point)
{
	m_Position = Point;
}

int32 CWoWPathNode::GetMapID() const
{
	return m_MapID;
}

void CWoWPathNode::SetMapID(int32 MapID)
{
	m_MapID = MapID;
}

uint32 CWoWPathNode::GetTimeDelay() const
{
	return m_TimeDelay;
}

void CWoWPathNode::SetTimeDelay(uint32 TimeDelay)
{
	m_TimeDelay = TimeDelay;
}
