#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "WoWGameObject.h"
#include "../Helpers/Spline/Path.h"

// FORWARD BEGIN
class CWoWWorld;
// FORWARD END

class ZN_API WoWGameObjectMOTransport
	: public WoWGameObject
{
public:
	WoWGameObjectMOTransport(IScene& Scene, CWoWWorld& WoWWorld, CWoWGuid Guid);
	virtual ~WoWGameObjectMOTransport();

	std::shared_ptr<CWoWPath> GetWoWPath() const { return m_WoWPath; }
	uint32 GetPathID() const;

	virtual void OnValueUpdated(uint16 index) override;

	// IClientCacheGameobjectResponseListener
	void OnTemplate(CWoWGuid::EntryType_t Entry, const std::shared_ptr<SGameObjectQueryResult>& QueryResult) override;

	// ISceneNode
	void Update(const UpdateEventArgs& e) override;

public:
	static std::shared_ptr<WoWGameObjectMOTransport> Create(CWoWWorld& WoWWorld, IScene& Scene, CWoWGuid Guid);
	virtual void Destroy() override;

private:
	std::string m_TransportName;

	std::shared_ptr<CWoWPath> m_WoWPath;
	uint32 m_PathID;
	float m_PathProgress;
};

#endif
