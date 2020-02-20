#pragma once

#include "Liquid/Liquid.h"

#include "WMO_Group_Headers.h"
#include "WMO_Part_Material.h"

// FORWARD BEGIN
class CWMO;
class WMO_Group;
// FORWARD END

class ZN_API CWMO_Liquid
	: public CLiquid
{
public:
	CWMO_Liquid(IRenderDevice& RenderDevice, const CWMO& WMOObject, const WMO_Group& WMOGroupObject, const std::shared_ptr<IByteBuffer>& Bytes, const SWMO_Group_MLIQDef& LiquidHeader);
	virtual ~CWMO_Liquid();

	const SWMO_Group_MLIQDef& GetHeader() const { return m_LiquidHeader; }

private:
	const CWMO&         m_WMOObject;
	const WMO_Group&    m_WMOGroupObject;
	SWMO_Group_MLIQDef  m_LiquidHeader;
};