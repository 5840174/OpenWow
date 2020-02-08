#pragma once

#include "WMO.h"
#include "WMO_Group_Instance.h"
#include "WMO_Doodad_Instance.h"
#include "WMO_Liquid_Instance.h"

class CWMO_Base_Instance 
	: public SceneNode3D
{
public:
	typedef std::vector<CWMO_Group_Instance*> GroupInstances;

public:
	CWMO_Base_Instance(std::string _wmoName);
	virtual ~CWMO_Base_Instance();

	void Initialize();

	void                                            CreateInstances();

	// CWMO_Base_Instance
	void                                            setWMO(std::shared_ptr<CWMO> _model);
	std::shared_ptr<CWMO>                           getWMO() const;

	void AddGroupInstance(CWMO_Group_Instance* _group) { m_GroupInstances.push_back(_group); }
	GroupInstances& getGroupInstances() { return m_GroupInstances; }

	void AddOutdoorGroupInstance(CWMO_Group_Instance* _group) { m_OutdoorGroupInstances.push_back(_group); }
	GroupInstances& getGroupOutdoorInstances() { return m_OutdoorGroupInstances; }

	const vec3* getVerts() const { return m_ConvertedVerts.data(); }

	bool                                            Load(IRenderDevice& RenderDevice);

	// SceneNode3D
	std::string                              GetName() const override
	{
		return "WMO '" + m_WMOName + "'";
	}

	void                                            UpdateCamera(const ICameraComponent3D* camera) override;

	void                                            Accept(IVisitor* visitor) override;

protected:
	std::string                                     m_WMOName;
	std::shared_ptr<CWMO>                           m_WMO;
	//SWMO_Doodad_SetInfo                           m_DoodadSetInfo;

	std::vector<vec3>                               m_ConvertedVerts;
	
	GroupInstances  m_GroupInstances;
	GroupInstances  m_OutdoorGroupInstances;
};