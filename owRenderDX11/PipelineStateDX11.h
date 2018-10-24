#pragma once

#include "BlendStateDX11.h"
#include "RasterizerStateDX11.h"
#include "DepthStencilStateDX11.h"

class PipelineStateDX11 : public PipelineState
{
public:
	PipelineStateDX11(ID3D11Device2* pDevice);
	virtual ~PipelineStateDX11();

	virtual void SetBlendState(const BlendState& blendState);
	virtual BlendState& GetBlendState();

	virtual void SetRasterizerState(const RasterizerState& rasterizerState);
	virtual RasterizerState& GetRasterizerState();

	virtual void SetDepthStencilState(const DepthStencilState& depthStencilState);
	virtual DepthStencilState& GetDepthStencilState();

	virtual void SetRenderTarget(std::shared_ptr<RenderTarget> renderTarget);
	virtual std::shared_ptr<RenderTarget> GetRenderTarget() const;

	virtual void Bind();
	virtual void UnBind();

protected:

private:
	ATL::CComPtr< ID3D11Device2 > m_pDevice;
	ATL::CComPtr< ID3D11DeviceContext2> m_pDeviceContext;

	BlendStateDX11 m_BlendState;
	RasterizerStateDX11 m_RasterizerState;
	DepthStencilStateDX11 m_DepthStencilState;
	std::shared_ptr<RenderTarget> m_RenderTarget;
};