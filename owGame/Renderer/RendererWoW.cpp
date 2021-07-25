#include "stdafx.h"

// General
#include "RendererWoW.h"

// Additional
#include "RenderPassCreateTypelessList.h"

#include "RenderPass_Update.h"
#include "RenderPass_LiquidList.h"
#include "RenderPass_M2List.h"
#include "RenderPass_M2InstancedList.h"
#include "RenderPass_M2ParticlesList.h"
#include "RenderPass_MapChunkList.h"
#include "RenderPass_SkyList.h"
#include "RenderPass_WDLList.h"
#include "RenderPass_WMOList.h"

#include "RenderPass_M2CollisionDebug.h"
#include "RenderPass_WMOPortalDebug.h"

CRendererWoW::CRendererWoW(IBaseManager& BaseManager, IScene& Scene)
	: RendererBase(BaseManager, Scene)
{
	InitEGxBlend(GetRenderDevice());
}

CRendererWoW::~CRendererWoW()
{
}


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
	Add3DPass(MakeShared(CRenderPass_WMOList, *this, GetRenderDevice(), m_SceneListTypelessPass)->ConfigurePipeline(OutputRenderTarget));
	

	Add3DPass(MakeShared(CRenderPass_M2InstancedList, *this, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::All)->ConfigurePipeline(OutputRenderTarget));
	Add3DPass(MakeShared(CRenderPass_M2List, *this, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::All)->ConfigurePipeline(OutputRenderTarget));
	
	//Add3DPass(MakeShared(CRenderPass_M2InstancedList, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::Opaque)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_M2List, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::Opaque)->ConfigurePipeline(OutputRenderTarget));
	
	//Add3DPass(MakeShared(CRenderPass_M2InstancedList, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::Transperent)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_M2List, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::Transperent)->ConfigurePipeline(OutputRenderTarget));
	
	Add3DPass(MakeShared(CRenderPass_LiquidList, GetRenderDevice(), m_SceneListTypelessPass)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_M2ParticlesList, GetRenderDevice(), m_SceneListTypelessPass)->ConfigurePipeline(OutputRenderTarget));



	//
	// DEBUG
	//
	//Add3DPass(MakeShared(CDebugPass, GetRenderDevice(), GetScene())->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CDrawBonesPass, m_Scene)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CDrawBoundingBoxPass, GetRenderDevice(), GetScene())->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_WMOPortalDebug, GetScene())->ConfigurePipeline(OutputRenderTarget));
	Add3DPass(MakeShared(CRenderPass_M2CollisionDebug, GetScene())->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CDrawLightFrustumPass, m_RenderDevice, m_Scene)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CPassDrawNormals, m_RenderDevice, m_Scene)->ConfigurePipeline(OutputRenderTarget));


	//
	// UI
	//
	AddUIPass(MakeShared(CUIControlPass, GetRenderDevice(), GetScene())->ConfigurePipeline(OutputRenderTarget));
	AddUIPass(MakeShared(CUIFontPass, GetRenderDevice(), GetScene())->ConfigurePipeline(OutputRenderTarget));
	//AddUIPass(MakeShared(CUIControlDebugBounds, GetRenderDevice(), GetScene())->ConfigurePipeline(OutputRenderTarget));

	//auto consolePass = MakeShared(CUIConsolePass, GetRenderDevice());
	//consolePass->ConfigurePipelines(OutputRenderTarget);
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
	Add3DPass(MakeShared(CRenderPass_WMOList, *this, GetRenderDevice(), m_SceneListTypelessPass)->ConfigurePipeline(gbufferRenderTarget));

	Add3DPass(MakeShared(CRenderPass_M2List, *this, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::All)->ConfigurePipeline(gbufferRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_M2List, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::Opaque)->ConfigurePipeline(gbufferRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_M2List, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::Transperent)->ConfigurePipeline(gbufferRenderTarget));

	Add3DPass(MakeShared(CRenderPass_M2InstancedList, *this, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::All)->ConfigurePipeline(gbufferRenderTarget));
	///Add3DPass(MakeShared(CRenderPass_M2InstancedList, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::Opaque)->ConfigurePipeline(gbufferRenderTarget));
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
	Add3DPass(MakeShared(CRenderPass_M2InstancedList, *this, GetRenderDevice(), m_SceneListTypelessPass, ERenderPassM2DrawMode::Transperent)->ConfigurePipeline(outputRenderTargetWithCustomDepth));


	Add3DPass(MakeShared(CRenderPass_M2ParticlesList, GetRenderDevice(), m_SceneListTypelessPass)->ConfigurePipeline(outputRenderTargetWithCustomDepth));

	//
	// DEBUG
	//
	//Add3DPass(MakeShared(CDebugPass, m_RenderDevice, m_Scene)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CDrawBonesPass, m_Scene)->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CDrawBoundingBoxPass, GetRenderDevice(), GetScene())->ConfigurePipeline(OutputRenderTarget));
	//Add3DPass(MakeShared(CRenderPass_WMOPortalDebug, GetScene())->ConfigurePipeline(outputRenderTargetWithCustomDepth));



	//
	// UI
	//
	AddUIPass(MakeShared(CUIControlPass, GetRenderDevice(), GetScene())->ConfigurePipeline(OutputRenderTarget));
	AddUIPass(MakeShared(CUIFontPass, GetRenderDevice(), GetScene())->ConfigurePipeline(OutputRenderTarget));
	//AddUIPass(MakeShared(CUIControlDebugBounds, GetRenderDevice(), GetScene())->ConfigurePipeline(OutputRenderTarget));
}



//
// Public
//

std::shared_ptr<IBlendState> CRendererWoW::GetEGxBlend(uint32 Index) const
{
	return m_EGxBlendStates.at(Index);
}



//
// Private
//
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
		0, 0, 0, 0, 32, 8);
	auto m_DepthStencilTexture = GetRenderDevice().GetObjectsFactory().CreateTexture2D(RenderTarget->GetViewport().GetWidth(), RenderTarget->GetViewport().GetHeight(), 1, depthStencilTextureFormat);

	auto rt = GetRenderDevice().GetObjectsFactory().CreateRenderTarget();
	rt->AttachTexture(IRenderTarget::AttachmentPoint::Color0, m_Texture0);
	rt->AttachTexture(IRenderTarget::AttachmentPoint::Color1, m_Texture1);
	rt->AttachTexture(IRenderTarget::AttachmentPoint::Color2, m_Texture2);
	rt->AttachTexture(IRenderTarget::AttachmentPoint::Color3, m_Texture3);
	rt->AttachTexture(IRenderTarget::AttachmentPoint::DepthStencil, m_DepthStencilTexture);
	return rt;
}

void CRendererWoW::InitEGxBlend(IRenderDevice& RenderDevice)
{
	for (uint32 i = 0; i < 14; i++)
	{
		if (i == 11)
			continue;

		std::shared_ptr<IBlendState> blendState = RenderDevice.GetObjectsFactory().CreateBlendState();
		blendState->SetBlendMode(GetEGxBlendMode(i));
		m_EGxBlendStates[i] = blendState;
	}
}

IBlendState::BlendMode CRendererWoW::GetEGxBlendMode(uint32 Index)
{
	switch (Index)
	{
		case 0: // Opaque
			return IBlendState::BlendMode(false, false,
				IBlendState::BlendFactor::One, IBlendState::BlendFactor::Zero);
			break;

		case 1: // AlphaKey
			return IBlendState::BlendMode(false, false,
				IBlendState::BlendFactor::One, IBlendState::BlendFactor::Zero);
			break;

		case 2: // Alpha
			return IBlendState::BlendMode(true, false,
				IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::OneMinusSrcAlpha,
				IBlendState::BlendOperation::Add,
				IBlendState::BlendFactor::One, IBlendState::BlendFactor::OneMinusSrcAlpha);
			break;

		case 3: // Add
			return IBlendState::BlendMode(true, false,
				IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::One,
				IBlendState::BlendOperation::Add,
				IBlendState::BlendFactor::Zero, IBlendState::BlendFactor::One);
			break;

		case 4: // Mod
			return IBlendState::BlendMode(true, false,
				IBlendState::BlendFactor::DstColor, IBlendState::BlendFactor::Zero,
				IBlendState::BlendOperation::Add,
				IBlendState::BlendFactor::DstAlpha, IBlendState::BlendFactor::Zero);
			break;

		case 5: // Mod2x
			return IBlendState::BlendMode(true, false,
				IBlendState::BlendFactor::DstColor, IBlendState::BlendFactor::SrcColor,
				IBlendState::BlendOperation::Add,
				IBlendState::BlendFactor::DstAlpha, IBlendState::BlendFactor::SrcAlpha);
			break;

		case 6: // ModAdd
			return IBlendState::BlendMode(true, false,
				IBlendState::BlendFactor::DstColor, IBlendState::BlendFactor::One,
				IBlendState::BlendOperation::Add,
				IBlendState::BlendFactor::DstAlpha, IBlendState::BlendFactor::One);
			break;

		case 7: // InvSrcAlphaAdd
			return IBlendState::BlendMode(true, false,
				IBlendState::BlendFactor::OneMinusSrcAlpha, IBlendState::BlendFactor::One,
				IBlendState::BlendOperation::Add,
				IBlendState::BlendFactor::OneMinusSrcAlpha, IBlendState::BlendFactor::One);
			break;

		case 8: // InvSrcAlphaOpaque
			return IBlendState::BlendMode(true, false,
				IBlendState::BlendFactor::OneMinusSrcAlpha, IBlendState::BlendFactor::Zero,
				IBlendState::BlendOperation::Add,
				IBlendState::BlendFactor::OneMinusSrcAlpha, IBlendState::BlendFactor::Zero);
			break;

		case 9: // SrcAlphaOpaque
			return IBlendState::BlendMode(true, false,
				IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::Zero,
				IBlendState::BlendOperation::Add,
				IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::Zero);
			break;

		case 10: // NoAlphaAdd
			return IBlendState::BlendMode(true, false,
				IBlendState::BlendFactor::One, IBlendState::BlendFactor::One,
				IBlendState::BlendOperation::Add,
				IBlendState::BlendFactor::Zero, IBlendState::BlendFactor::One);

		case 11: // ConstantAlpha
			//(true, GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA, GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
			throw CException("Constant alpha (11) EGxBlend doesn't support");

		case 12: // Screen
			return IBlendState::BlendMode(true, false,
				IBlendState::BlendFactor::OneMinusDstColor, IBlendState::BlendFactor::One,
				IBlendState::BlendOperation::Add,
				IBlendState::BlendFactor::One, IBlendState::BlendFactor::Zero);
			break;

		case 13: // BlendAdd
			return IBlendState::BlendMode(true, false,
				IBlendState::BlendFactor::One, IBlendState::BlendFactor::OneMinusSrcAlpha,
				IBlendState::BlendOperation::Add,
				IBlendState::BlendFactor::One, IBlendState::BlendFactor::OneMinusSrcAlpha);
			break;

		default:
			_ASSERT(false);
	}

	_ASSERT(false);
	return IBlendState::BlendMode();
}
