#pragma once

class CWoWPathNode
{
public:
	CWoWPathNode();
	CWoWPathNode(const glm::vec3& Point);
	virtual ~CWoWPathNode();

	const glm::vec3& GetPosition() const;
	void SetPosition(const glm::vec3& Point);

private:
	glm::vec3 m_Position;
};