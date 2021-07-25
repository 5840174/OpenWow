#pragma once

#include "RenderPassPipelinedProcessTypelessList.h"

#include "M2/M2_Base_Instance.h"

// FORWARD BEGIN
class CRendererWoW;
// FORWARD END

namespace
{
	struct __declspec(novtable, align(16)) ZN_API M2PerObject
	{
		M2PerObject(const glm::mat4& Model, const glm::vec4& Color)
			: Model(Model)
			, Color(Color)
		{}
		glm::mat4 Model;
		glm::vec4 Color;
	};
}

enum class ZN_API ERenderPassM2DrawMode
{
	Opaque,
	Transperent,
	All
};

class ZN_API CRenderPass_M2List
	: public CRenderPassPipelinedProcessTypelessList
{
public:
	CRenderPass_M2List(CRendererWoW& RendererWoW, IRenderDevice& RenderDevice, const std::shared_ptr<IRenderPassCreateTypelessList>& CreateTypelessList, ERenderPassM2DrawMode DrawMode = ERenderPassM2DrawMode::All);
	virtual ~CRenderPass_M2List();

	// CRenderPass_M2
	void DoRenderM2Model(const CM2_Base_Instance* M2SceneNode, const CM2_Skin* M2Model, UINT InstancesCnt = UINT32_MAX);

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget) override;

    // IVisitor
    virtual EVisitResult Visit(const std::shared_ptr<ISceneNode>& node) override;
	virtual EVisitResult Visit(const std::shared_ptr<IModel>& Model) override;

protected:
	CRendererWoW& m_RendererWoW;

	ERenderPassM2DrawMode m_DrawMode;

	const CM2_Base_Instance* m_CurrentM2Instance;

	std::shared_ptr<IConstantBuffer> m_M2PerObjectConstantBuffer;
	IShaderParameter* m_ShaderM2PerObjectParameter;
	IShaderParameter* m_ShaderM2GeometryParameter;
	IShaderParameter* m_ShaderM2GeometryBonesParameter;
};
