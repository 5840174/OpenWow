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
	: public CSceneNodeProxie
{
public:
#ifdef USE_M2_MODELS
	typedef std::vector<std::shared_ptr<CWMO_Doodad_Instance>> DoodadInstances;
#endif
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

#ifdef USE_M2_MODELS
    void addDoodadInstance(std::shared_ptr<CWMO_Doodad_Instance> _doodad) { m_Doodads.push_back(_doodad); }
    const DoodadInstances& getDoodadsInstances() { return m_Doodads; }
#endif

    void addLiquidInstance(std::shared_ptr<CWMO_Liquid_Instance> _liquid) { m_Liquids.push_back(_liquid); }
    const LuqidInstances& getLiquidInstances() { return m_Liquids; }

	// SceneNode3D
	bool Accept(IVisitor* visitor) override;

private:
	const std::shared_ptr<WMO_Group> m_Object;

	bool                             m_PortalsVis;
	bool                             m_Calculated;

#ifdef USE_M2_MODELS
	DoodadInstances                  m_Doodads;
#endif
	LuqidInstances                   m_Liquids;
};