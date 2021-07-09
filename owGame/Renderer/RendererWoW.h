#pragma once

class ZN_API CRendererWoW
	: public RendererBase
{
public:
	CRendererWoW(IBaseManager& BaseManagerl, IScene& Scene);
	virtual ~CRendererWoW();

	void InitializeForward(std::shared_ptr<IRenderTarget> OutputRenderTarget);
	void InitializeDeffered(std::shared_ptr<IRenderTarget> OutputRenderTarget);

private:
	std::shared_ptr<IRenderTarget> CreateGBuffer(std::shared_ptr<IRenderTarget> RenderTarget);

private:
	std::shared_ptr<IRenderPassCreateTypelessList> m_SceneListTypelessPass;
	std::shared_ptr<CPassDeffered_ShadowMaps> m_ShadowMapsPass;
	std::shared_ptr<CPassDeffered_RenderUIQuad> m_Deffered_UIQuadPass;
};