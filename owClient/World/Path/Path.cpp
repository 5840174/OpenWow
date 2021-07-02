#include "stdafx.h"

// General
#include "Path.h"

CWoWPath::CWoWPath()
{}

CWoWPath::~CWoWPath()
{}

const std::vector<std::shared_ptr<CWoWPathNode>>& CWoWPath::GetPathNodes() const
{
	return m_Nodes;
}

size_t CWoWPath::GetPathNodesCount() const
{
	return m_Nodes.size();
}

std::shared_ptr<CWoWPathNode> CWoWPath::GetPath(size_t Index) const
{
	if (Index >= m_Nodes.size())
		throw CException("CWoWPath::GetPath: Index '%d' is out of bounds.", Index);
	return m_Nodes.at(Index);
}

void CWoWPath::AddPathNode(std::shared_ptr<CWoWPathNode> PathNode)
{
	m_Nodes.push_back(PathNode);
}
