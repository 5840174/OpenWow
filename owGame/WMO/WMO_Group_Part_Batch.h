#pragma once

#ifdef USE_WMO_MODELS

#include "WMO_Headers.h"

// FORWARD BEGIN
class CWMO;
class WMO_Group;
// FORWARD END

class WMO_Group_Part_Batch 
	: public ModelProxie
{
public:
	WMO_Group_Part_Batch(IRenderDevice& RenderDevice, const CWMO& WMOModel, const SWMO_Group_BatchDef& WMOGroupBatchProto);
	virtual ~WMO_Group_Part_Batch();

	// ModelProxie
	bool Render(const ShaderMap& Shaders) const override;

private:
	BoundingBox m_Bounds;

private:
	const CWMO& m_ParentWMO;
	const SWMO_Group_BatchDef m_WMOGroupBatchProto;
};

/*
struct WMO_Group_Part_BatchCompare
{
	bool operator() (const std::shared_ptr<WMO_Group_Part_Batch> left, const std::shared_ptr<WMO_Group_Part_Batch> right) const
	{
		return left->getBlendMode() < right->getBlendMode();
	}
};
*/

#endif
