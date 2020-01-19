#pragma once

#include "WMO_Headers.h"

#include "WMO_Part_Material.h"

// FORWARD BEGIN
class CWMO;
class WMO_Group;
// FORWARD END

class WMO_Group_Part_Batch 
	: public MeshProxie
{
public:
	WMO_Group_Part_Batch(const std::weak_ptr<const CWMO> _parentWMO, std::shared_ptr<IMesh> _mesh, const SWMO_Group_BatchDef& _proto);

	uint32 getBlendMode() const { return m_WMOMaterial->getBlendMode(); }

	// MeshProxie
	bool Render(const RenderEventArgs* renderEventArgs, const IConstantBuffer* perObject, SGeometryPartParams GeometryPartParams = SGeometryPartParams()) override;

private:
	const SWMO_Group_BatchDef                   m_Proto;
	std::shared_ptr<const WMO_Part_Material>    m_WMOMaterial;
	BoundingBox                                 m_Bounds;

	// Parent
	const std::weak_ptr<const CWMO>              m_ParentWMO;
	const std::weak_ptr<const WMO_Group>        m_ParentGroup;
};

struct WMO_Group_Part_BatchCompare
{
	bool operator() (const std::shared_ptr<WMO_Group_Part_Batch> left, const std::shared_ptr<WMO_Group_Part_Batch> right) const
	{
		return left->getBlendMode() < right->getBlendMode();
	}
};