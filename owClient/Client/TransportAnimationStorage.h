#pragma once

class CowClient_DBCTransport
{
public:
	struct STransportAnimation
	{
		uint32 Time;
		glm::vec3 Position;
	};

public:
	CowClient_DBCTransport(IBaseManager& BaseManager);
	virtual ~CowClient_DBCTransport();

	void Initialize();
	const std::vector<STransportAnimation>& GetTransportAnimations(uint32 Entry) const;

private:
	std::vector<STransportAnimation> m_EmptyAnimations;
	std::map<uint32, std::vector<STransportAnimation>> m_TransportRecords;

	IBaseManager& m_BaseManager;
};