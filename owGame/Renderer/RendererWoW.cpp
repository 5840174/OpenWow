#include "stdafx.h"

// General
#include "RendererWoW.h"

// Additional
#include "RenderPassCreateTypelessList.h"

#include "RenderPass_Liquid.h"
#include "RenderPass_M2.h"
#include "RenderPass_M2Instanced.h"
#include "RenderPass_MapChunk.h"
#include "RenderPass_MapTile.h"
#include "RenderPass_Sky.h"
#include "RenderPass_WDL.h"
#include "RenderPass_WMO.h"

#include "RenderPass_Update.h"
#include "RenderPass_LiquidList.h"
#include "RenderPass_M2List.h"
#include "RenderPass_M2InstancedList.h"
#include "RenderPass_M2ParticlesList.h"
#include "RenderPass_MapChunkList.h"
#include "RenderPass_SkyList.h"
#include "RenderPass_WDLList.h"
#include "RenderPass_WMOList.h"

#include "RenderPass_WMOPortalDebug.h"

CRendererWoW::CRendererWoW(IBaseManager& BaseManager, IScene& Scene)
	: RendererBase(BaseManager, Scene)
{
}

CRendererWoW::~CRendererWoW()
{
}


/*
	//Add3DPass(MakeShared(CRenderPass_Sky, GetScene())->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_WDL, GetScene())->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_MapChunk, GetScene())->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_WMO, GetScene())->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_M2, GetScene(), ERenderPassM2DrawMode::All)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_M2, GetScene(), false)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_M2_Instanced, GetScene(), ERenderPassM2DrawMode::All)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_Liquid, GetScene())->ConfigurePipeline(OutputRenderTarget));

*/


void CRendererWoW::InitializeForward(std::shared_ptr<IRenderTarget> OutputRenderTarget)
{
	//
	// BEFORE SCENE
	//
	Add3DPass(MakeShared(ClearRenderTargetPass, GetRenderDevice(), OutputRenderTarget));


	//
	// SCENE
	//
	m_SceneListTypelessPass = MakeShared(CRenderPassCreateTypelessList, GetRenderDevice(), GetScene());
	Add3DPass(m_SceneListTypelessPass);

	Add3DPass(MakeShared(CRenderPass_Update, GetRenderDevice(), m_SceneListTypelessPass));

	Add3DPass(MakeShared(CRenderPass_SkyList, GetRenderDevice(), m_SceneListTypelessPass)->ConfigurePipeline(OutputRenderTarget));
	Add3DPass(MakeShared(CRenderPass_WDLList, GetRenderDevice(), m_SceneListTypelessPass)->ConfigurePipeline(OutputRenderTarget));
	Add3DPass(MakeShared(CRenderPass_MapChunkList, GetRenderDevice(), m_SceneListTypelessPass)->ConfigurePipeline(OutputRenderTarget));
	Add3DPass(MakeShared(CRenderPass_WMOList, GetRenderDevice(), m_SceneListTypelessPass)->ConfigurePipeline(OutputRenderTarget));
	
	Add3DPass(MakeShared(CRenderPass_M2List, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::All)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_M2List, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::Opaque)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_M2List, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::Transperent)->ConfigurePipeline(OutputRenderTarget));
	
	//Add3DPass(MakeShared(CRenderPass_M2InstancedList, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::All)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_M2InstancedList, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::Opaque)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_M2InstancedList, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::Transperent)->ConfigurePipeline(OutputRenderTarget));
	
	Add3DPass(MakeShared(CRenderPass_LiquidList, GetRenderDevice(), m_SceneListTypelessPass)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_M2ParticlesList, GetRenderDevice(), m_SceneListTypelessPass)->ConfigurePipeline(OutputRenderTarget));



	//
	// DEBUG
	//
	//Add3DPass(MakeShared(CDebugPass, m_RenderDevice, m_Scene)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CDrawBonesPass, m_Scene)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CDrawBoundingBoxPass, GetRenderDevice(), GetScene())->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_WMOPortalDebug, GetScene())->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CDrawLightFrustumPass, m_RenderDevice, m_Scene)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CPassDrawNormals, m_RenderDevice, m_Scene)->ConfigurePipeline(OutputRenderTarget));


	//
	// UI
	//
	AddUIPass(MakeShared(CUIControlPass, GetRenderDevice(), GetScene())->ConfigurePipeline(OutputRenderTarget));
	AddUIPass(MakeShared(CUIFontPass, GetRenderDevice(), GetScene())->ConfigurePipeline(OutputRenderTarget));
	//AddUIPass(MakeShared(CUIControlDebugBounds, GetRenderDevice(), GetScene())->ConfigurePipeline(OutputRenderTarget));

	//std::shared_ptr<CUIConsolePass> consolePass = MakeShared(CUIConsolePass, m_RenderDevice);
	//consolePass->Configure(OutputRenderTarget);
	//AddUIPass(consolePass);
}

void CRendererWoW::InitializeDeffered(std::shared_ptr<IRenderTarget> OutputRenderTarget)
{
	auto gbufferRenderTarget = CreateGBuffer(OutputRenderTarget);

	auto outputRenderTargetWithCustomDepth = GetRenderDevice().GetObjectsFactory().CreateRenderTarget();
	outputRenderTargetWithCustomDepth->AttachTexture(IRenderTarget::AttachmentPoint::Color0, OutputRenderTarget->GetTexture(IRenderTarget::AttachmentPoint::Color0));
	outputRenderTargetWithCustomDepth->AttachTexture(IRenderTarget::AttachmentPoint::DepthStencil, gbufferRenderTarget->GetTexture(IRenderTarget::AttachmentPoint::DepthStencil));



	//
	// BEFORE SCENE
	//
	Add3DPass(MakeShared(ClearRenderTargetPass, GetRenderDevice(), OutputRenderTarget));
	Add3DPass(MakeShared(ClearRenderTargetPass, GetRenderDevice(), gbufferRenderTarget));



	//
	// DEBUG
	//
	m_SceneListTypelessPass = MakeShared(CRenderPassCreateTypelessList, GetRenderDevice(), GetScene());
	Add3DPass(m_SceneListTypelessPass);

	Add3DPass(MakeShared(CRenderPass_Update, GetRenderDevice(), m_SceneListTypelessPass));

	Add3DPass(MakeShared(CRenderPass_SkyList, GetRenderDevice(), m_SceneListTypelessPass)->ConfigurePipeline(gbufferRenderTarget));
	Add3DPass(MakeShared(CRenderPass_WDLList, GetRenderDevice(), m_SceneListTypelessPass)->ConfigurePipeline(gbufferRenderTarget));
	Add3DPass(MakeShared(CRenderPass_MapChunkList, GetRenderDevice(), m_SceneListTypelessPass)->ConfigurePipeline(gbufferRenderTarget));
	Add3DPass(MakeShared(CRenderPass_WMOList, GetRenderDevice(), m_SceneListTypelessPass)->ConfigurePipeline(gbufferRenderTarget));

	//Add3DPass(MakeShared(CRenderPass_M2List, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::All)->ConfigurePipeline(gbufferRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_M2List, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::Opaque)->ConfigurePipeline(gbufferRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_M2List, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::Transperent)->ConfigurePipeline(gbufferRenderTarget));

	//Add3DPass(MakeShared(CRenderPass_M2InstancedList, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::All)->ConfigurePipeline(gbufferRenderTarget));
	Add3DPass(MakeShared(CRenderPass_M2InstancedList, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::Opaque)->ConfigurePipeline(gbufferRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_M2InstancedList, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::Transperent)->ConfigurePipeline(gbufferRenderTarget));




	// DEFFERED STUFF
	m_ShadowMapsPass = MakeShared(CPassDeffered_ShadowMaps, GetRenderDevice(), m_SceneListTypelessPass);
	m_ShadowMapsPass->CreateShadowPipeline();
	Add3DPass(m_ShadowMapsPass);

	m_Deffered_UIQuadPass = MakeShared(CPassDeffered_RenderUIQuad, GetRenderDevice(), m_SceneListTypelessPass, gbufferRenderTarget, m_ShadowMapsPass);
	m_Deffered_UIQuadPass->ConfigurePipeline(OutputRenderTarget);
	Add3DPass(m_Deffered_UIQuadPass);



	//
	// AFTER DEFFERED
	//
	Add3DPass(MakeShared(CRenderPass_LiquidList, GetRenderDevice(), m_SceneListTypelessPass)->ConfigurePipeline(outputRenderTargetWithCustomDepth));
	//Add3DPass(MakeShared(CRenderPass_M2ParticlesList, GetRenderDevice(), m_SceneListTypelessPass)->ConfigurePipeline(outputRenderTargetWithCustomDepth));

	//Add3DPass(MakeShared(CRenderPass_M2List, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::Transperent)->ConfigurePipeline(outputRenderTargetWithCustomDepth));
	Add3DPass(MakeShared(CRenderPass_M2InstancedList, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::Transperent)->ConfigurePipeline(outputRenderTargetWithCustomDepth));



	//
	// DEBUG
	//
	//Add3DPass(MakeShared(CDebugPass, m_RenderDevice, m_Scene)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CDrawBonesPass, m_Scene)->ConfigurePipeline(OutputRenderTarget));
	Add3DPass(MakeShared(CDrawBoundingBoxPass, GetRenderDevice(), GetScene())->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_WMOPortalDebug, GetScene())->ConfigurePipeline(outputRenderTargetWithCustomDepth));



	//
	// UI
	//
	AddUIPass(MakeShared(CUIControlPass, GetRenderDevice(), GetScene())->ConfigurePipeline(OutputRenderTarget));
	AddUIPass(MakeShared(CUIFontPass, GetRenderDevice(), GetScene())->ConfigurePipeline(OutputRenderTarget));
	//AddUIPass(MakeShared(CUIControlDebugBounds, GetRenderDevice(), GetScene())->ConfigurePipeline(OutputRenderTarget));
}

std::shared_ptr<IRenderTarget> CRendererWoW::CreateGBuffer(std::shared_ptr<IRenderTarget> RenderTarget)
{
	ITexture::TextureFormat colorTextureFormat
	(
		ITexture::Components::RGBA,
		ITexture::Type::Float,
		RenderTarget->GetSamplesCount(),
		16, 16, 16, 16, 0, 0
	);

	auto m_Texture0 = GetRenderDevice().GetObjectsFactory().CreateTexture2D(RenderTarget->GetViewport().GetWidth(), RenderTarget->GetViewport().GetHeight(), 1, colorTextureFormat);
	auto m_Texture1 = GetRenderDevice().GetObjectsFactory().CreateTexture2D(RenderTarget->GetViewport().GetWidth(), RenderTarget->GetViewport().GetHeight(), 1, colorTextureFormat);
	auto m_Texture2 = GetRenderDevice().GetObjectsFactory().CreateTexture2D(RenderTarget->GetViewport().GetWidth(), RenderTarget->GetViewport().GetHeight(), 1, colorTextureFormat);
	auto m_Texture3 = GetRenderDevice().GetObjectsFactory().CreateTexture2D(RenderTarget->GetViewport().GetWidth(), RenderTarget->GetViewport().GetHeight(), 1, colorTextureFormat);

	// Depth/stencil buffer
	ITexture::TextureFormat depthStencilTextureFormat(
		ITexture::Components::DepthStencil,
		ITexture::Type::UnsignedNormalized,
		RenderTarget->GetSamplesCount(),
		0, 0, 0, 0, 24, 8);
	auto m_DepthStencilTexture = GetRenderDevice().GetObjectsFactory().CreateTexture2D(RenderTarget->GetViewport().GetWidth(), RenderTarget->GetViewport().GetHeight(), 1, depthStencilTextureFormat);

	auto rt = GetRenderDevice().GetObjectsFactory().CreateRenderTarget();
	rt->AttachTexture(IRenderTarget::AttachmentPoint::Color0, m_Texture0);
	rt->AttachTexture(IRenderTarget::AttachmentPoint::Color1, m_Texture1);
	rt->AttachTexture(IRenderTarget::AttachmentPoint::Color2, m_Texture2);
	rt->AttachTexture(IRenderTarget::AttachmentPoint::Color3, m_Texture3);
	rt->AttachTexture(IRenderTarget::AttachmentPoint::DepthStencil, m_DepthStencilTexture);
	return rt;
}
