#pragma once

#include "Point.h"

class CWoWPath
{
public:
	CWoWPath();
	virtual ~CWoWPath();

	const std::vector<std::shared_ptr<CWoWPathNode>>& GetPathNodes() const;
	size_t GetPathNodesCount() const;
	std::shared_ptr<CWoWPathNode> GetPath(size_t Index) const;
	void AddPathNode(std::shared_ptr<CWoWPathNode> PathNode);

private:
	std::vector<std::shared_ptr<CWoWPathNode>> m_Nodes;

};