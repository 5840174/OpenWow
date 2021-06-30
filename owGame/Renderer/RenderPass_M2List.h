#pragma once

#include "RenderPassPipelinedProcessTypelessList.h"
#include "RenderPass_M2.h"

class ZN_API CRenderPass_M2List
	: public CRenderPassPipelinedProcessTypelessList
{
public:
	CRenderPass_M2List(IRenderDevice& RenderDevice, const std::shared_ptr<IRenderPassCreateTypelessList>& CreateTypelessList, ERenderPassM2DrawMode DrawMode = ERenderPassM2DrawMode::All);
	virtual ~CRenderPass_M2List();

	// CRenderPass_M2
	void DoRenderM2Model(const CM2_Base_Instance* M2SceneNode, const CM2_Skin* M2Model, UINT InstancesCnt = UINT32_MAX);

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget) override;

    // IVisitor
    virtual EVisitResult Visit(const std::shared_ptr<ISceneNode>& node) override;
	virtual EVisitResult Visit(const std::shared_ptr<IModel>& Model) override;

protected:
	ERenderPassM2DrawMode m_DrawMode;

	const CM2_Base_Instance* m_CurrentM2Model;

	std::shared_ptr<IConstantBuffer> m_M2PerObjectConstantBuffer;
	IShaderParameter* m_ShaderM2PerObjectParameter;
	IShaderParameter* m_ShaderM2GeometryParameter;
	IShaderParameter* m_ShaderM2GeometryBonesParameter;
};
