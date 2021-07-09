#pragma once

class CWoWPathNode
{
public:
	CWoWPathNode();
	CWoWPathNode(const glm::vec3& Point);
	virtual ~CWoWPathNode();

	const glm::vec3& GetPosition() const;
	void SetPosition(const glm::vec3& Point);

	int32 GetMapID() const;
	void SetMapID(int32 MapID);

	uint32 GetTimeDelay() const;
	void SetTimeDelay(uint32 TimeDelay);

private:
	glm::vec3 m_Position;
	int32 m_MapID;
	uint32 m_TimeDelay;
};