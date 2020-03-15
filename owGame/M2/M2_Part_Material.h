#pragma once

#include "M2_Types.h"

class CM2_Part_Material
{
public:
	CM2_Part_Material(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const CM2& M2Object, const SM2_Material& M2Material);
	virtual ~CM2_Part_Material();

	SM2_Material::BlendModes getBlendMode() const { return m_M2BlendMode; }

	const std::shared_ptr<IDepthStencilState>& GetDepthStencilState() const { return m_DepthStencilState; }
	const std::shared_ptr<IBlendState>& GetBlendState() const { return m_BlendState; };
	const std::shared_ptr<IRasterizerState>& GetRasterizerState() const { return m_RasterizerState; };

private:
	SM2_Material::BlendModes m_M2BlendMode;

	std::shared_ptr<IDepthStencilState> m_DepthStencilState;
	std::shared_ptr<IBlendState> m_BlendState;
	std::shared_ptr<IRasterizerState> m_RasterizerState;

private:
	const CM2& m_M2Object;
};