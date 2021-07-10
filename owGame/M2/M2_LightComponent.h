#pragma once

// FORWARD BEGIN
class CM2;
class CM2_Base_Instance;
// FORWARD END

#include "M2_Light.h"

class ZN_API CM2LightComponent
	: public CSceneNodeComponentBase
	, public ILightComponent3D
{
public:
	CM2LightComponent(const CM2_Base_Instance& OwnerNode);
	virtual ~CM2LightComponent();

	// ILightComponent3D
	void SetLight(std::shared_ptr<ILight> Light) override;
	std::shared_ptr<ILight> GetLight() const override;

	// ISceneNodeComponent
	void OnMessage(const ISceneNodeComponent* Component, ComponentMessageType Message) override;
	void Update(const UpdateEventArgs& e) override;
	void Accept(IVisitor* visitor) override;

protected:
	const CM2_Base_Instance& GetM2OwnerNode() const;

private:
	std::vector<std::shared_ptr<CM2Light>> m_Lights;
};