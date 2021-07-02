#include "stdafx.h"

// General
#include "Point.h"

CWoWPathNode::CWoWPathNode()
	: m_Position(glm::vec3(0.0f))
{}

CWoWPathNode::CWoWPathNode(const glm::vec3 & Point)
	: m_Position(Point)
{}

CWoWPathNode::~CWoWPathNode()
{}

const glm::vec3 & CWoWPathNode::GetPosition() const
{
	return m_Position;
}

void CWoWPathNode::SetPosition(const glm::vec3 & Point)
{
	m_Position = Point;
}
