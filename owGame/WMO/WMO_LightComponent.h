#pragma once

// FORWARD BEGIN
class CWMO;
class CWMO_Base_Instance;
// FORWARD END

#include "WMO_Light.h"

class ZN_API CWMOLightComponent
	: public CSceneNodeComponentBase
	, public ILightComponent3D
{
public:
	CWMOLightComponent(const CWMO_Base_Instance& OwnerNode);
	virtual ~CWMOLightComponent();

	// ILightComponent3D
	void SetLight(std::shared_ptr<ILight> Light) override;
	std::shared_ptr<ILight> GetLight() const override;

	// ISceneNodeComponent
	void OnMessage(const ISceneNodeComponent* Component, ComponentMessageType Message) override;
	void Update(const UpdateEventArgs& e) override;
	void Accept(IVisitor* visitor) override;

protected:
	const CWMO_Base_Instance& GetWMOOwnerNode() const;

private:
	std::vector<std::shared_ptr<CWMOLight>> m_Lights;
};