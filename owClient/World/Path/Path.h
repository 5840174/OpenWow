#pragma once

#include "PathNode.h"

class CWoWPath
{
public:
	CWoWPath();
	virtual ~CWoWPath();

	std::vector<std::shared_ptr<CWoWPathNode>> GetPathNodes() const;
	size_t GetPathNodesCount() const;
	std::shared_ptr<CWoWPathNode> GetPathNode(size_t Index) const;
	void AddPathNode(std::shared_ptr<CWoWPathNode> PathNode);

	int32 GetDuration() const;
	void SetDuration(int32 Duration);

	int32 GetCurrTime() const;
	void SetCurrTime(int32 CurrTime);
	void AddCurrTime(int32 Value);

	glm::vec3 GetPositionByCurrTime() const;
	glm::vec3 GetNextNodePosition() const;

private:
	void CalculateTimes();
	size_t GetCurrentNodeIndex() const;

private:
	struct SPathNode
	{
		explicit SPathNode(std::shared_ptr<CWoWPathNode> Node_)
			: Node(Node_)
			, DistanceToPreviousPoint(0.0f)
			, Timestamp(0)
		{}

		std::shared_ptr<CWoWPathNode> Node;
		float DistanceToPreviousPoint;
		int32 Timestamp;
	};

private:
	std::vector<SPathNode> m_Nodes;
	int32 m_FullPathDuration;
	float m_FullPathLength;
	int32 m_CurrTime;
};