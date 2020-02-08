#pragma once

#ifdef USE_M2_MODELS

#include "WMO_Headers.h"

#include "WMO_Group.h"

class ZN_API CWMO_Doodad_Instance : public CM2_Base_Instance
{
public:
	CWMO_Doodad_Instance(std::string _m2Name, const std::weak_ptr<const WMO_Group> _parentGroup, uint32 _index);
	virtual ~CWMO_Doodad_Instance();

    void Initialize(const SWMO_Doodad_PlacementInfo& _placement);

	void setPortalVisibility(bool _visibility) { m_PortalVis = _visibility; }
		
	// ILoadable
	uint32 getPriority() const override { return CM2_Base_Instance::getPriority() + 1; };

	// SceneNode3D
	void Initialize() override;

	bool Accept(IVisitor* visitor) override;

private:
	const std::weak_ptr<const WMO_Group>	m_ParentGroup;
	const uint32		                    m_Index;
	bool				                    m_PortalVis;
};

#endif
