#pragma once

#include "Liquid.h"

class Liquid_Instance : public SceneNode3D
{
public:
	Liquid_Instance();
	virtual ~Liquid_Instance();

    void Initialize(std::shared_ptr<CLiquid> _liquidObject, vec3 _position);

	// SceneNode3D
	bool Accept(IVisitor* visitor) override;

private:
	const CGroupQuality& m_QualitySettings;
};