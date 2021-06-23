#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "WoWUnit.h"

class ZN_API WoWPlayer
	: public WoWUnit
{
public:
	WoWPlayer(IScene& Scene, CWoWObjectGuid Guid);
	virtual ~WoWPlayer();

public:
	static std::shared_ptr<WoWPlayer> Create(IScene& Scene, CWoWObjectGuid Guid);

private:
	float transportX;
	float transportY;
	float transportZ;
	float transportO;
	uint64 transportGUID;
};

#endif