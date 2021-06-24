#pragma once

class ZN_API CRendererWoW
	: public RendererBase
{
public:
	CRendererWoW(IBaseManager& BaseManagerl, IScene& Scene);
	virtual ~CRendererWoW();

	void Initialize(std::shared_ptr<IRenderTarget> OutputRenderTarget);

private:
	std::shared_ptr<IRenderPassCreateTypelessList> m_SceneListTypelessPass;
};