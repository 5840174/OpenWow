#pragma once

#ifdef USE_WMO_MODELS

#include "WMOGroup_Headers.h"

// FORWARD BEGIN
class CWMO;
class CWMOGroup;
// FORWARD END

class WMO_Group_Part_Batch 
	: public ModelProxie
{
public:
	enum EBatchType : uint8
	{
		BatchType_Trans = 0,
		BatchType_Int,
		BatchType_Ext
	};

public:
	WMO_Group_Part_Batch(IRenderDevice& RenderDevice, const CWMO& WMOModel, const SWMOGroup_MOBA& WMOGroupBatchProto);
	virtual ~WMO_Group_Part_Batch();

	void SetBatchType(EBatchType BatchType) { m_BatchType = BatchType; }
	EBatchType GetBatchType() const { return m_BatchType; }

	// ModelProxie
	bool Render(const ShaderMap& Shaders) const override;
	void Accept(IVisitor* visitor) override;

private:
	BoundingBox m_Bounds;
	EBatchType m_BatchType;

private:
	const CWMO& m_ParentWMO;
	//const SWMOGroup_MOBA m_WMOGroupBatchProto;
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
