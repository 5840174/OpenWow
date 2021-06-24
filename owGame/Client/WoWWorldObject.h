#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "WoWObject.h"

class ZN_API CWoWWorldObject
	: public WoWObject
{
public:
	CWoWWorldObject(IScene& Scene, CWoWObjectGuid Guid);
	virtual ~CWoWWorldObject();

	virtual void Update(const UpdateEventArgs& e);

	

	const IBaseManager& GetBaseManager() const { return m_BaseManager; }

	uint32 MapID;
	glm::vec3 Position;
	float Orientation;

	void CommitPositionAndRotation();

protected:
	std::shared_ptr<ISceneNode> m_HiddenNode;
	bool m_HiddenNodeDirty;

private:
	const IBaseManager& m_BaseManager;
};

#endif