#pragma once

#include "WMO.h"
#include "WMO_Group_Instance.h"
#include "WMO_Doodad_Instance.h"
#include "WMO_Liquid_Instance.h"

class ZN_API CWMO_Base_Instance 
	: public SceneNode3D
	, public CLoadableObject
{
public:
	typedef std::vector<CWMO_Group_Instance*> GroupInstances;

public:
	CWMO_Base_Instance(const CWMO& WMOObject);
	virtual ~CWMO_Base_Instance();

	void CreateInstances();

	// CLoadableObject
	bool Load() override;

	// CWMO_Base_Instance
	const CWMO& getWMO() const;

	void AddGroupInstance(CWMO_Group_Instance* _group) { m_GroupInstances.push_back(_group); }
	const GroupInstances& getGroupInstances() const { return m_GroupInstances; }

	void AddOutdoorGroupInstance(CWMO_Group_Instance* _group) { m_OutdoorGroupInstances.push_back(_group); }
	const GroupInstances& getGroupOutdoorInstances() const { return m_OutdoorGroupInstances; }

	const vec3* getVerts() const { return m_ConvertedVerts.data(); }

	// SceneNode3D
	virtual void Initialize() override;
	std::string GetName() const override;
	void UpdateCamera(const ICameraComponent3D* camera) override;
	void Accept(IVisitor* visitor) override;

protected:
	const CWMO&                           m_WMOObject;
	//SWMO_Doodad_SetInfo                           m_DoodadSetInfo;

	std::vector<vec3>                               m_ConvertedVerts;
	
	GroupInstances  m_GroupInstances;
	GroupInstances  m_OutdoorGroupInstances;
};