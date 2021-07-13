#pragma once

#ifdef USE_WMO_MODELS

#include "WMO.h"
#include "WMO_Group_Instance.h"
#include "WMO_Doodad_Instance.h"
#include "WMO_Liquid_Instance.h"

#include "WMO_LightComponent.h"
#include "WMO_PortalsComponent.h"

class ZN_API CWMO_Base_Instance 
	: public CSceneNode
	, public CLoadableObject
{
public:
	typedef std::vector<std::weak_ptr<CWMO_Group_Instance>> GroupInstances;

public:
	CWMO_Base_Instance(IScene& Scene, const std::shared_ptr<CWMO>& WMOObject);
	virtual ~CWMO_Base_Instance();

	// CWMO_Base_Instance
	const CWMO& GetWMO() const;
	
	virtual uint16 GetDoodadSetIndex() const;
	bool IsDoodadInSet(uint16 doodadIndex) const;

	void AddGroupInstance(const std::weak_ptr<CWMO_Group_Instance>& _group) { m_GroupInstances.push_back(_group); }
	const GroupInstances& getGroupInstances() const { return m_GroupInstances; }

	//void AddOutdoorGroupInstance(const std::weak_ptr<CWMO_Group_Instance>& _group) { m_OutdoorGroupInstances.push_back(_group); }
	//const GroupInstances& getGroupOutdoorInstances() const { return m_OutdoorGroupInstances; }

	// ISceneNode
	void Initialize() override;
	void Update(const UpdateEventArgs& e) override;
	void Accept(IVisitor* visitor) override;

	// ILoadable
	bool Load() override;
	bool Delete() override;

protected:
	GroupInstances  m_GroupInstances;
	//GroupInstances  m_OutdoorGroupInstances;

	std::shared_ptr<CWMOLightComponent> m_LightComponent;
#ifdef USE_WMO_PORTALS_CULLING
	std::shared_ptr<CWMO_PortalsComponent> m_PortalsComponent;
#endif

private:
	std::shared_ptr<CWMO> m_WMOObject;
};

#endif
