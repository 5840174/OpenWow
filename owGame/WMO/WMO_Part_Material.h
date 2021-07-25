#pragma once

#ifdef USE_WMO_MODELS

#include "WMO_Headers.h"

// FORWARD BEGIN
class CWorldClient;
class CWMO;
// FORWARD END

namespace
{
__declspec(align(16)) struct WMO_Part_MaterialMaterialProperties
{
	WMO_Part_MaterialMaterialProperties()
		: BlendMode(0)
		, MOCVExists(0)
	{}

	uint32 BlendMode;
	uint32 MOCVExists;
	glm::vec2   m_Pad;
	//-------------------------- ( 32 bytes )
};
}

class WMO_Part_Material 
	: public MaterialProxieT<WMO_Part_MaterialMaterialProperties>
{
public:
	WMO_Part_Material(IRenderDevice& RenderDevice, const CWMO& WMOModel, const SWMO_MOMT& WMOMaterialProto);
	virtual ~WMO_Part_Material();

	uint32 GetBlendMode() const;
	void SetBlendMode(uint32 BlendMode);


	void SetMOCVExists(uint32 MOCVExists);

	const std::shared_ptr<IDepthStencilState>& GetDepthStencilState() const { return m_DepthStencilState; }
	//const std::shared_ptr<IBlendState>& GetBlendState() const { return m_BlendState; };
	const std::shared_ptr<IRasterizerState>& GetRasterizerState() const { return m_RasterizerState; };

private:
	std::shared_ptr<IDepthStencilState> m_DepthStencilState;
	//std::shared_ptr<IBlendState>        m_BlendState;
	std::shared_ptr<IRasterizerState>   m_RasterizerState;

private:
	const CWMO& m_WMO;
};

#endif
