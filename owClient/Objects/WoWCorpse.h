#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "Base/WoWWorldObject.h"

// FORWARD BEGIN
class CWorldServer;
// FORWARD END

class ZN_API CowServerCorpse
	: public CowServerWorldObject
{
public:
	CowServerCorpse(IScene& Scene, CWorldServer& WoWWorld, CowGuid Guid);
	virtual ~CowServerCorpse();

public:
	static std::shared_ptr<CowServerCorpse> Create(CWorldServer& WoWWorld, IScene& Scene, CowGuid Guid);

private:

};

#endif