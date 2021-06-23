#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "WoWWorldObject.h"

class ZN_API WoWCorpse
	: public WorldObject
{
public:
	WoWCorpse(IScene& Scene, CWoWObjectGuid Guid);
	virtual ~WoWCorpse();

public:
	static std::shared_ptr<WoWCorpse> Create(IScene& Scene, CWoWObjectGuid Guid);

private:

};

#endif