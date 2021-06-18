#pragma once

#include "M2/M2_Base_Instance.h"

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

enum class ERenderPassM2DrawMode
{
	Opaque,
	Transperent,
	All
};

class ZN_API CRenderPass_M2 
	: public Base3DPass
{
public:
	CRenderPass_M2(IScene& Scene, ERenderPassM2DrawMode DrawMode = ERenderPassM2DrawMode::All);
	virtual ~CRenderPass_M2();

	// CRenderPass_M2
	void DoRenderM2Model(const CM2_Base_Instance* M2SceneNode, const CM2_Skin* M2Model, ERenderPassM2DrawMode DrawMode, UINT InstancesCnt = UINT32_MAX);

	// IRenderPassPipelined
	virtual std::shared_ptr<IRenderPassPipelined> ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget) override;

    // IVisitor
    virtual EVisitResult Visit(const std::shared_ptr<ISceneNode>& node) override;
	virtual EVisitResult Visit(const std::shared_ptr<IModel>& Model) override;

protected:
	std::shared_ptr<IPropertyT<float>> m_ADT_MDX_Distance;
	ERenderPassM2DrawMode m_DrawMode;

	std::shared_ptr<CM2_Base_Instance> m_CurrentM2Model;

	std::shared_ptr<IConstantBuffer> m_M2PerObjectConstantBuffer;
	IShaderParameter* m_ShaderM2PerObjectParameter;
	IShaderParameter* m_ShaderM2GeometryParameter;
	IShaderParameter* m_ShaderM2GeometryBonesParameter;
};
