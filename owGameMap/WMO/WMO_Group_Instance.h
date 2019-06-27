#pragma once

#include "WMO_Group.h"

// FORWARD BEGIN
class CWMO_Base_Instance;
class CWMO_Doodad_Instance;
class CWMO_Liquid_Instance;
// FORWARD END

class CWMO_Group_Instance : public SceneNode3D
{
public:
	typedef std::vector<std::shared_ptr<CWMO_Doodad_Instance>> DoodadInstances;
	typedef std::vector<std::shared_ptr<CWMO_Liquid_Instance>> LuqidInstances;
public:
	CWMO_Group_Instance(const std::shared_ptr<WMO_Group> _object);
	virtual ~CWMO_Group_Instance();

    void Initialize();

    std::shared_ptr<WMO_Group> getObject() const { return m_Object; }

    void SetPortalVisible(bool Value);
    bool GetPortalVisible() const;

    void SetPortalCalculated(bool Value);
    bool GetPortalCalculated() const;

    void addDoodadInstance(std::shared_ptr<CWMO_Doodad_Instance> _doodad) { m_Doodads.push_back(_doodad); }
    const DoodadInstances& getDoodadsInstances() { return m_Doodads; }

    void addLiquidInstance(std::shared_ptr<CWMO_Liquid_Instance> _liquid) { m_Liquids.push_back(_liquid); }
    const LuqidInstances& getLiquidInstances() { return m_Liquids; }

	// SceneNode3D
	bool Accept(std::shared_ptr<IVisitor> visitor) override;

private:
	const std::shared_ptr<WMO_Group> m_Object;

	bool                             m_PortalsVis;
	bool                             m_Calculated;

	DoodadInstances                  m_Doodads;
	LuqidInstances                   m_Liquids;
};