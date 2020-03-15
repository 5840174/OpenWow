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

	const std::shared_ptr<IDepthStencilState>& GetDepthStencilState() const { return m_DepthStencilState; }
	const std::shared_ptr<IBlendState>& GetBlendState() const { return m_BlendState; };
	const std::shared_ptr<IRasterizerState>& GetRasterizerState() const { return m_RasterizerState; };

    void UpdateConstantBuffer() const override;

private:
	__declspec(align(16)) struct MaterialProperties
	{
		MaterialProperties()
			: BlendMode(0)
			, MOCVExists(0)
		{}

		uint32 BlendMode;
		uint32 MOCVExists;
		glm::vec2   m_Pad;
		//-------------------------- ( 32 bytes )
	};
	MaterialProperties*            m_pProperties;

	std::shared_ptr<IDepthStencilState> m_DepthStencilState;
	std::shared_ptr<IBlendState>        m_BlendState;
	std::shared_ptr<IRasterizerState>   m_RasterizerState;

private:
	const CWMO& m_WMOModel;
};