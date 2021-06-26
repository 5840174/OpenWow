#pragma once

class CTransportAnimationStorage
{
public:
	struct STransportAnimation
	{
		uint32 Time;
		glm::vec3 Position;
	};

public:
	CTransportAnimationStorage(IBaseManager& BaseManager);
	virtual ~CTransportAnimationStorage();

	void Initialize();
	const std::vector<STransportAnimation>& GetTransportAnimations(uint32 Entry) const;

private:
	std::vector<STransportAnimation> m_EmptyAnimations;
	std::map<uint32, std::vector<STransportAnimation>> m_TransportRecords;

	IBaseManager& m_BaseManager;
};