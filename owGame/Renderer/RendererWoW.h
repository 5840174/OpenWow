#pragma once

class ZN_API CRendererWoW
	: public RendererBase
{
public:
	CRendererWoW(IBaseManager& BaseManager, IScene& Scene);
	virtual ~CRendererWoW();

	void InitializeForward(std::shared_ptr<IRenderTarget> OutputRenderTarget);
	void InitializeDeffered(std::shared_ptr<IRenderTarget> OutputRenderTarget);


public:
	std::shared_ptr<IBlendState>   GetEGxBlend(uint32 Index) const;


private:
	std::shared_ptr<IRenderTarget> CreateGBuffer(std::shared_ptr<IRenderTarget> RenderTarget);


private: // Blend mode
	void                           InitEGxBlend(IRenderDevice& RenderDevice);
	IBlendState::BlendMode         GetEGxBlendMode(uint32 Index);


private:
	std::shared_ptr<IRenderPassCreateTypelessList> m_SceneListTypelessPass;
	std::shared_ptr<CPassDeffered_ShadowMaps> m_ShadowMapsPass;
	std::shared_ptr<CPassDeffered_RenderUIQuad> m_Deffered_UIQuadPass;

	std::map<uint32, std::shared_ptr<IBlendState>> m_EGxBlendStates;
};