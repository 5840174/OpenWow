#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "WoWObject.h"

// FORWARD BEGIN
class CWorldServer;
class CowServerGameObject_MOTransport;
// FORWARD END

class ZN_API CowServerWorldObject
	: public CowServerObject
{
public:
	CowServerWorldObject(IScene& Scene, CWorldServer& WoWWorld, CowGuid Guid);
	virtual ~CowServerWorldObject();

	void Update(const UpdateEventArgs& e);

	virtual void OnHiddenNodePositionChanged();

	// Position
	uint32 MapID;
	glm::vec3 Position;
	float Orientation;

	void CommitPositionAndRotation();

	// Transport
	std::weak_ptr<CowServerGameObject_MOTransport> TransportObject;
	CowGuid TransportID;
	glm::vec3 PositionTransportOffset;
	float OrientationTransportOffset;

	IScene& GetScene() const { return m_Scene; }
	CWorldServer& GetWoWWorld() const { return m_ServerWorld; }
	const IBaseManager& GetBaseManager() const { return m_BaseManager; }

protected:
	bool m_IsTranslationDirty;

private:
	IScene& m_Scene;
	CWorldServer& m_ServerWorld;
	const IBaseManager& m_BaseManager;
};

#endif