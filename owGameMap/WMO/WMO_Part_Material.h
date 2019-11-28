#pragma once

#include "WMO_Headers.h"

// FORWARD BEGIN
class CWMO;
// FORWARD END

class WMO_Part_Material : public MaterialWrapper
{
public:
	WMO_Part_Material(const std::weak_ptr<const CWMO> _parentWMO, const SWMO_MaterialDef& _proto);
	virtual ~WMO_Part_Material();
	
	uint32 getBlendMode() const { return m_Proto.blendMode; }

    void UpdateConstantBuffer() const override;

private:
	const SWMO_MaterialDef         m_Proto;

	__declspec(align(16)) struct MaterialProperties
	{
		uint32 m_BlendMode;
		vec3   m_Pad;
		//-------------------------- ( 32 bytes )
	};
	MaterialProperties*            m_pProperties;

	// Parent
	const std::weak_ptr<const CWMO> m_ParentWMO;
	const CGroupQuality*           m_QualitySettings;
};