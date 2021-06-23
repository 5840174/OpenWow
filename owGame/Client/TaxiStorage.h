#pragma once

class CTaxiStorage
{
public:
	struct STaxiPathNode
	{
		glm::vec3 Position;
		float Delay;
	};

public:
	CTaxiStorage(IBaseManager& BaseManager);
	virtual ~CTaxiStorage();

	void Initialize();
	const std::vector<STaxiPathNode>& GetPathNodes(uint32 PathID) const;

private:
	std::vector<STaxiPathNode> m_EmptyPathNodes;
	std::map<uint32, std::vector<STaxiPathNode>> m_PathNodes;

	IBaseManager& m_BaseManager;
};
