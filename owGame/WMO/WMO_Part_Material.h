#pragma once

#include "WMO_Headers.h"

// FORWARD BEGIN
class CWMO;
// FORWARD END

class WMO_Part_Material 
	: public MaterialProxie
{
public:
	WMO_Part_Material(IRenderDevice& RenderDevice, const CWMO& WMOModel, const SWMO_MaterialDef& WMOMaterialProto);
	virtual ~WMO_Part_Material();
	
	uint32 getBlendMode() const { return m_WMOMaterialProto.blendMode; }

    void UpdateConstantBuffer() const override;

private:
	__declspec(align(16)) struct MaterialProperties
	{
		uint32 m_BlendMode;
		glm::vec3   m_Pad;
		//-------------------------- ( 32 bytes )
	};
	MaterialProperties*            m_pProperties;

private:
	const CWMO& m_WMOModel;
	const SWMO_MaterialDef m_WMOMaterialProto;
};