#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "WoWGameObject.h"
#include "ClientCacheTemplates.h"

// FORWARD BEGIN
class CWoWWorld;
// FORWARD END


class ZN_API WoWGameObjectMOTransport
	: public WoWGameObject
{
public:
	WoWGameObjectMOTransport(IScene& Scene, CWoWWorld& WoWWorld, CWoWObjectGuid Guid);
	virtual ~WoWGameObjectMOTransport();

	// ISceneNode
	void Update(const UpdateEventArgs& e) override;

public:
	static std::shared_ptr<WoWGameObjectMOTransport> Create(CWoWWorld& WoWWorld, IScene& Scene, CWoWObjectGuid Guid);
	virtual void AfterCreate(IScene& Scene) override;
	virtual void Destroy() override;

private:
	std::string m_TransportName;
	float m_PathProgress;
};

#endif
