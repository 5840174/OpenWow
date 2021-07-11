#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "WoWObject.h"

// FORWARD BEGIN
class CWoWWorld;
class WoWGameObjectMOTransport;
// FORWARD END

class ZN_API CWoWWorldObject
	: public WoWObject
{
public:
	CWoWWorldObject(IScene& Scene, CWoWWorld& WoWWorld, CWoWGuid Guid);
	virtual ~CWoWWorldObject();

	virtual void Update(const UpdateEventArgs& e);

	virtual void OnHiddenNodePositionChanged();

	// Position
	uint32 MapID;
	glm::vec3 Position;
	float Orientation;

	void CommitPositionAndRotation();

	// Transport
	std::weak_ptr<WoWGameObjectMOTransport> TransportObject;
	CWoWGuid TransportID;
	glm::vec3 PositionTransportOffset;
	float OrientationTransportOffset;

	IScene& GetScene() const { return m_Scene; }
	CWoWWorld& GetWoWWorld() const { return m_WoWWorld; }
	const IBaseManager& GetBaseManager() const { return m_BaseManager; }

protected:
	bool m_IsTranslationDirty;

private:
	IScene& m_Scene;
	CWoWWorld& m_WoWWorld;
	const IBaseManager& m_BaseManager;
};

#endif