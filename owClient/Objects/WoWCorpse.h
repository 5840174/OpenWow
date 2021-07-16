#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "Base/WoWWorldObject.h"

// FORWARD BEGIN
class CowServerWorld;
// FORWARD END

class ZN_API CowServerCorpse
	: public CowServerWorldObject
{
public:
	CowServerCorpse(IScene& Scene, CowServerWorld& WoWWorld, CowGuid Guid);
	virtual ~CowServerCorpse();

public:
	static std::shared_ptr<CowServerCorpse> Create(CowServerWorld& WoWWorld, IScene& Scene, CowGuid Guid);

private:

};

#endif