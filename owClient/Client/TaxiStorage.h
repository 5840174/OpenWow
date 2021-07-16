#pragma once

class CowClient_DBCTaxi
{
public:
	struct STaxiPathNode
	{
		uint32 MapID;
		glm::vec3 Position;
		float Delay;
	};

public:
	CowClient_DBCTaxi(IBaseManager& BaseManager);
	virtual ~CowClient_DBCTaxi();

	void Initialize();
	const std::vector<STaxiPathNode>& GetPathNodes(uint32 PathID) const;

private:
	std::vector<STaxiPathNode> m_EmptyPathNodes;
	std::map<uint32, std::vector<STaxiPathNode>> m_PathNodes;

	IBaseManager& m_BaseManager;
};
