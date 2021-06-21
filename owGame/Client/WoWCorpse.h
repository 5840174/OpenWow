#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "WoWObject.h"

class ZN_API WoWCorpse
	: public WoWObject
{
public:
	WoWCorpse(ObjectGuid Guid);
	virtual ~WoWCorpse();

public:
	static std::shared_ptr<WoWCorpse> Create(IScene& Scene, ObjectGuid Guid);

private:

};

#endif