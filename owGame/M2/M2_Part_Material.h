#pragma once

#include "M2_Types.h"

class CM2_Part_Material
{
public:
	CM2_Part_Material(const M2& M2Object, const SM2_Material& M2Material);
	virtual ~CM2_Part_Material();

	void Set() const;

	SM2_Material::BlendModes getBlendMode() const { return m_M2BlendMode; }

	IDepthStencilState::DepthMode GetDepthMode() const;
	IBlendState::BlendMode GetBlendMode() const;
	IRasterizerState::CullMode GetCullMode() const;

private:
	bool m_IsLightingDisable;
	bool m_IsFogDisable;
	IRasterizerState::CullMode m_CullMode;

	bool m_DepthTest;
	IDepthStencilState::DepthWrite m_DepthWrite;

	SM2_Material::BlendModes m_M2BlendMode;

private:
	const M2& m_M2Object;
};