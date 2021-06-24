#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "WoWUnit.h"

// FORWARD BEGIN
class CWoWWorld;
// FORWARD END

class ZN_API WoWPlayer
	: public WoWUnit
{
public:
	WoWPlayer(IScene& Scene, CWoWWorld& WoWWorld, CWoWObjectGuid Guid);
	virtual ~WoWPlayer();

public:
	static std::shared_ptr<WoWPlayer> Create(CWoWWorld& WoWWorld, IScene& Scene, CWoWObjectGuid Guid);

private:
	//float transportX;
	//float transportY;
	//float transportZ;
	//float transportO;
	//uint64 transportGUID;
};

#endif