#pragma once

#include "RenderPassPipelinedProcessTypelessList.h"

class ZN_API CRenderPass_M2ParticlesList
	: public CRenderPassPipelinedProcessTypelessList
{
public:
	CRenderPass_M2ParticlesList(IRenderDevice& RenderDevice, const std::shared_ptr<IRenderPassCreateTypelessList>& CreateTypelessList);
	virtual ~CRenderPass_M2ParticlesList();

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget) override final;

	// IVisitor
	EVisitResult Visit(const std::shared_ptr<ISceneNode>& SceneNode) override;
	EVisitResult Visit(const std::shared_ptr<IParticleSystem>& ParticlesSystem) override;

private:
	std::shared_ptr<IStructuredBuffer> m_GeomParticlesBuffer;
	IShaderParameter*                  m_GeomShaderParticlesBufferParameter;
	
private:
	std::shared_ptr<IGeometry>         m_Geometry;
};