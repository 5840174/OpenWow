#pragma once

#include "WMO.h"
#include "WMO_Group_Instance.h"
#include "WMO_Doodad_Instance.h"
#include "WMO_Liquid_Instance.h"

class CWMO_Base_Instance : public SceneNode3D
{
public:
	CWMO_Base_Instance(std::string _wmoName);
	virtual ~CWMO_Base_Instance();

	void CreateInstances();

	// CWMO_Base_Instance
	void setWMO(std::shared_ptr<WMO> _model);
	std::shared_ptr<WMO> getWMO() const;

	void AddGroupInstance(std::shared_ptr<CWMO_Group_Instance> _group) { m_GroupInstances.push_back(_group); }
	std::vector<std::shared_ptr<CWMO_Group_Instance>>& getGroupInstances() { return m_GroupInstances; }

	void AddOutdoorGroupInstance(std::shared_ptr<CWMO_Group_Instance> _group) { m_OutdoorGroupInstances.push_back(_group); }
	std::vector<std::shared_ptr<CWMO_Group_Instance>>& getGroupOutdoorInstances() { return m_OutdoorGroupInstances; }

	const vec3* getVerts() const { return m_ConvertedVerts.data(); }

	// ILoadable
	bool Load() override;
	bool Delete() override;
	uint32 getPriority() const override { return 2; };

	// SceneNode3D
	void UpdateCamera(const Camera* camera) override;
	bool Accept(IVisitor& visitor) override;

protected:
	std::string                                        m_WMOName;
	std::shared_ptr<WMO>                               m_WMO;
	//SWMO_Doodad_SetInfo                                m_DoodadSetInfo;

	std::vector<vec3>                                  m_ConvertedVerts;
	
	std::vector<std::shared_ptr<CWMO_Group_Instance>>  m_GroupInstances;
	std::vector<std::shared_ptr<CWMO_Group_Instance>>  m_OutdoorGroupInstances;
};