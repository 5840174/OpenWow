#pragma once

#include "MapChunk.h"

class CADT_Liquid 
	: public CLiquid
{
public:
	CADT_Liquid(IBaseManager* BaseManager, uint32 _x, uint32 _y);
    virtual ~CADT_Liquid();

	void CreateFromMCLQ(std::shared_ptr<IFile>, ADT_MCNK_Header header);
};