#pragma once

#include "WoWObject.h"

class ZN_API WoWCorpse
	: public WoWObject
{
public:
	WoWCorpse(ObjectGuid Guid);
	virtual ~WoWCorpse();

public:
	static std::shared_ptr<WoWCorpse> Create(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene, ObjectGuid Guid);

private:

};
