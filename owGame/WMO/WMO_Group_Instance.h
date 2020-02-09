#pragma once

#include "WMO_Group.h"

// FORWARD BEGIN
class CWMO_Base_Instance;
#ifdef USE_M2_MODELS
class CWMO_Doodad_Instance;
#endif
class CWMO_Liquid_Instance;
// FORWARD END

class CWMO_Group_Instance 
	: public SceneNode3D
{
public:
#ifdef USE_M2_MODELS
	typedef std::vector<CWMO_Doodad_Instance*> DoodadInstances;
#endif
	typedef std::vector<CWMO_Liquid_Instance*> LuqidInstances;

public:
	CWMO_Group_Instance(const WMO_Group& WMOGroupObject);
	virtual ~CWMO_Group_Instance();

    void Initialize();

    const WMO_Group& getObject() const { return m_WMOGroupObject; }

    void SetPortalVisible(bool Value);
    bool GetPortalVisible() const;

    void SetPortalCalculated(bool Value);
    bool GetPortalCalculated() const;

#ifdef USE_M2_MODELS
    void addDoodadInstance(CWMO_Doodad_Instance* _doodad) { m_Doodads.push_back(_doodad); }
    const DoodadInstances& getDoodadsInstances() { return m_Doodads; }
#endif

    void addLiquidInstance(CWMO_Liquid_Instance* _liquid) { m_Liquids.push_back(_liquid); }
    const LuqidInstances& getLiquidInstances() { return m_Liquids; }

	// SceneNode3D
	std::string GetName() const override
	{
		return "WMOGroup '" + m_WMOGroupObject.m_GroupName + "'";
	}

	void Accept(IVisitor* visitor) override;

private:
	

	bool                             m_PortalsVis;
	bool                             m_Calculated;

#ifdef USE_M2_MODELS
	DoodadInstances                  m_Doodads;
#endif
	LuqidInstances                   m_Liquids;


private:
	const WMO_Group& m_WMOGroupObject;
};