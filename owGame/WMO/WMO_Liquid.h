#pragma once

#include "Liquid.h"

#include "WMO_Part_Material.h"

class CWMO_Liquid 
	: public CLiquid
{
public:
	CWMO_Liquid(IBaseManager* BaseManager, uint32 _x, uint32 _y);
	virtual ~CWMO_Liquid();

	void CreateFromWMO(std::shared_ptr<IFile> f, std::shared_ptr<const WMO_Part_Material> _material, std::shared_ptr<const DBC_LiquidTypeRecord> _liquidType, bool _indoor);
};