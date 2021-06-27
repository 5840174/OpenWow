#pragma once

#ifdef USE_WMO_MODELS

#include "../Liquid/Liquid.h"

#include "WMO_Group_Headers.h"
#include "WMO_Part_Material.h"

// FORWARD BEGIN
class CWMO;
class CWMO_Group;
// FORWARD END

class ZN_API CWMO_Liquid
	: public CLiquid
{
public:
	CWMO_Liquid(IRenderDevice& RenderDevice, const CWMO& WMOObject, const CWMO_Group& WMOGroupObject, const std::shared_ptr<IByteBuffer>& Bytes, const SWMO_Group_MLIQDef& LiquidHeader, const DBC_LiquidTypeRecord* LiquidType);
	virtual ~CWMO_Liquid();

	const SWMO_Group_MLIQDef& GetHeader() const { return m_LiquidHeader; }

private:
	const CWMO&         m_WMOObject;
	const CWMO_Group&    m_WMOGroupObject;
	SWMO_Group_MLIQDef  m_LiquidHeader;
};

#endif
