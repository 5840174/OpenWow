#pragma once

#include "M2/M2_Base_Instance.h"

struct __declspec(novtable, align(16)) ZN_API M2PerObject
{
	M2PerObject(const glm::mat4& Model, const glm::vec4& Color)
		: Model(Model)
		, Color(Color)
	{}
	glm::mat4 Model;
	glm::vec4 Color;
};

class ZN_API CRenderPass_M2 
	: public CBaseList3DPass
{
public:
	CRenderPass_M2(IRenderDevice& RenderDevice, const std::shared_ptr<CSceneCreateTypedListsPass>& SceneNodeListPass, bool OpaqueDraw);
	virtual ~CRenderPass_M2();

	// CRenderPass_M2
	void DoRenderM2Model(const CM2_Base_Instance* M2SceneNode, const CM2_Skin* M2Model, bool OpaqueDraw, UINT InstancesCnt = UINT32_MAX);

	// IRenderPassPipelined
	virtual std::shared_ptr<IRenderPassPipelined> CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport* Viewport) override;

    // IVisitor
    virtual EVisitResult Visit(const ISceneNode3D* node) override;
	virtual EVisitResult Visit(const IModel* Model) override;

protected:
	std::shared_ptr<ISettingT<float>> m_ADT_MDX_Distance;
	bool m_OpaqueDraw;

	const CM2_Base_Instance* m_CurrentM2Model;

	std::shared_ptr<IConstantBuffer> m_M2PerObjectConstantBuffer;
	IShaderParameter* m_ShaderM2PerObjectParameter;
	IShaderParameter* m_ShaderM2GeometryParameter;
	IShaderParameter* m_ShaderM2GeometryBonesParameter;
};
