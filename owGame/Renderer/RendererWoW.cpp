#include "stdafx.h"

// General
#include "RendererWoW.h"

// Additional
#include "RenderPass_Liquid.h"
#include "RenderPass_M2.h"
#include "RenderPass_M2Instanced.h"
#include "RenderPass_MapChunk.h"
#include "RenderPass_MapTile.h"
#include "RenderPass_Sky.h"
#include "RenderPass_WDL.h"
#include "RenderPass_WMO.h"

CRendererWoW::CRendererWoW(IBaseManager& BaseManager, IScene& Scene)
	: RendererBase(BaseManager, Scene)
{
}

CRendererWoW::~CRendererWoW()
{
}



void CRendererWoW::Initialize(std::shared_ptr<IRenderTarget> OutputRenderTarget)
{
	//
	// BEFORE SCENE
	//
	Add3DPass(MakeShared(ClearRenderTargetPass, GetRenderDevice(), OutputRenderTarget));


	//
	// SCENE
	//
	Add3DPass(MakeShared(CRenderPass_Sky, GetScene())->ConfigurePipeline(OutputRenderTarget));
	Add3DPass(MakeShared(CRenderPass_WDL, GetScene())->ConfigurePipeline(OutputRenderTarget));
	Add3DPass(MakeShared(CRenderPass_MapTile, GetScene())->ConfigurePipeline(OutputRenderTarget));
	Add3DPass(MakeShared(CRenderPass_ADT_MCNK, GetScene())->ConfigurePipeline(OutputRenderTarget));
	Add3DPass(MakeShared(CRenderPass_WMO, GetScene())->ConfigurePipeline(OutputRenderTarget));
	Add3DPass(MakeShared(CRenderPass_M2_Instanced, GetScene(), ERenderPassM2DrawMode::All)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_M2, GetScene(), ERenderPassM2DrawMode::All)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_M2, GetScene(), false)->ConfigurePipeline(OutputRenderTarget));
	Add3DPass(MakeShared(CRenderPass_Liquid, GetScene())->ConfigurePipeline(OutputRenderTarget));

	//
	// DEBUG
	//
	//Add3DPass(MakeShared(CDebugPass, m_RenderDevice, m_Scene)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CDrawBonesPass, m_Scene)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CDrawBoundingBoxPass, m_RenderDevice, m_Scene)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CDrawLightFrustumPass, m_RenderDevice, m_Scene)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CPassDrawNormals, m_RenderDevice, m_Scene)->ConfigurePipeline(OutputRenderTarget));


	//
	// UI
	//
	AddUIPass(MakeShared(CUIControlPass, GetRenderDevice(), GetScene())->ConfigurePipeline(OutputRenderTarget));
	AddUIPass(MakeShared(CUIFontPass, GetRenderDevice(), GetScene())->ConfigurePipeline(OutputRenderTarget));


	//std::shared_ptr<CUIConsolePass> consolePass = MakeShared(CUIConsolePass, m_RenderDevice);
	//consolePass->Configure(OutputRenderTarget);
	//AddUIPass(consolePass);
}
