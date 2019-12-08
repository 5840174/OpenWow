#pragma once

#include "WMO_Headers.h"

#include "WMO_Group.h"

class CWMO_Liquid_Instance 
	: public CSceneNodeProxie
{
public:
	CWMO_Liquid_Instance(const std::weak_ptr<const WMO_Group> _group);
	virtual ~CWMO_Liquid_Instance();

	// SceneNode3D
	bool Accept(IVisitor* visitor) override;

private:
	const std::weak_ptr<const WMO_Group> m_Group; // used for visibility
};