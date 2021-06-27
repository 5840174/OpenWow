#pragma once

#ifdef USE_WMO_MODELS

#include "WMO.h"
#include "WMO_Group_Instance.h"
#include "WMO_Doodad_Instance.h"
#include "WMO_Liquid_Instance.h"

class ZN_API CWMO_Base_Instance 
	: public CSceneNode
	, public CLoadableObject
{
public:
	typedef std::vector<std::weak_ptr<CWMO_Group_Instance>> GroupInstances;

public:
	CWMO_Base_Instance(IScene& Scene, const std::shared_ptr<CWMO>& WMOObject);
	virtual ~CWMO_Base_Instance();

	void CreateInstances();

	// CLoadableObject
	bool Load() override;
	bool Delete() override;

	// CWMO_Base_Instance
	const CWMO& getWMO() const;
	
	virtual uint16 GetDoodadSetIndex() const;
	bool IsDoodadInSet(uint16 doodadIndex) const;

	void AddGroupInstance(const std::weak_ptr<CWMO_Group_Instance>& _group) { m_GroupInstances.push_back(_group); }
	const GroupInstances& getGroupInstances() const { return m_GroupInstances; }

	//void AddOutdoorGroupInstance(const std::weak_ptr<CWMO_Group_Instance>& _group) { m_OutdoorGroupInstances.push_back(_group); }
	//const GroupInstances& getGroupOutdoorInstances() const { return m_OutdoorGroupInstances; }

	// SceneNode3D
	void Initialize() override;
	void Update(const UpdateEventArgs& e) override;
	void Accept(IVisitor* visitor) override;

protected:
	std::shared_ptr<CWMO> m_WMOObject;	
	GroupInstances  m_GroupInstances;
	//GroupInstances  m_OutdoorGroupInstances;
};

#endif
