#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "WoWGameObject.h"
#include "../Helpers/Spline/Path.h"

// FORWARD BEGIN
class CWorldServer;
// FORWARD END

class ZN_API CowServerGameObject_MOTransport
	: public CowServerGameObject
{
public:
	CowServerGameObject_MOTransport(IScene& Scene, CWorldServer& WoWWorld, CowGuid Guid);
	virtual ~CowServerGameObject_MOTransport();

	std::shared_ptr<CWoWPath> GetWoWPath() const { return m_WoWPath; }
	uint32 GetPathID() const;

	virtual void OnValueUpdated(uint16 index) override;

	// IClientCacheGameobjectResponseListener
	void OnTemplate(CowGuid::EntryType_t Entry, const std::shared_ptr<SGameObjectQueryResult>& QueryResult) override;

	// ISceneNode
	void Update(const UpdateEventArgs& e);

public:
	static std::shared_ptr<CowServerGameObject_MOTransport> Create(CWorldServer& WoWWorld, IScene& Scene, CowGuid Guid);
	virtual void Destroy() override;

private:
	std::string m_TransportName;

	std::shared_ptr<CWoWPath> m_WoWPath;
	uint32 m_PathID;
	float m_PathProgress;
};

#endif
