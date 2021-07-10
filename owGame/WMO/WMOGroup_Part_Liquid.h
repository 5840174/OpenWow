#pragma once

#ifdef USE_WMO_MODELS

#include "../Liquid/Liquid.h"

#include "WMOGroup_Headers.h"
#include "WMO_Part_Material.h"

// FORWARD BEGIN
class CWMO;
class CWMOGroup;
// FORWARD END

class ZN_API CWMOGroup_Part_Liquid
	: public CLiquid
{
public:
	CWMOGroup_Part_Liquid(IRenderDevice& RenderDevice, const CWMO& WMOObject, const CWMOGroup& WMOGroupObject, const std::shared_ptr<IByteBuffer>& Bytes, const SWMOGroup_MLIQ& LiquidHeader, const DBC_LiquidTypeRecord* LiquidType);
	virtual ~CWMOGroup_Part_Liquid();

	const SWMOGroup_MLIQ& GetHeader() const { return m_LiquidHeader; }

private:
	const CWMO&         m_WMOObject;
	const CWMOGroup&    m_WMOGroup;
	SWMOGroup_MLIQ  m_LiquidHeader;
};

#endif
