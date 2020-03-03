#pragma once

#include "M2_Types.h"

class CM2_Part_Material
{
public:
	CM2_Part_Material(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const M2& M2Object, const SM2_Material& M2Material);
	virtual ~CM2_Part_Material();

	SM2_Material::BlendModes getBlendMode() const { return m_M2BlendMode; }
	IBlendState::BlendMode CM2_Part_Material::GetBlendMode() const;

	const std::shared_ptr<IDepthStencilState>& GetDepthStencilState() const { return m_DepthStencilState; }
	const std::shared_ptr<IBlendState>& GetBlendState() const { return m_BlendState; };

private:
	bool m_IsLightingDisable;
	bool m_IsFogDisable;
	IRasterizerState::CullMode m_CullMode;

	bool m_DepthTest;
	IDepthStencilState::DepthWrite m_DepthWrite;

	SM2_Material::BlendModes m_M2BlendMode;

	std::shared_ptr<IDepthStencilState> m_DepthStencilState;
	std::shared_ptr<IBlendState> m_BlendState;
	std::shared_ptr<IRasterizerState> m_RasterizerState;

private:
	const M2& m_M2Object;
};