#pragma once

#include "M2/M2_Base_Instance.h"

class ZN_API CRenderPass_M2 
	: public CBaseList3DPass
{
public:
	CRenderPass_M2(IRenderDevice& RenderDevice, const std::shared_ptr<CSceneNodeListPass>& SceneNodeListPass, bool OpaqueDraw);
	virtual ~CRenderPass_M2();

	// CRenderPass_M2
	void DoRenderM2Model(const CM2_Base_Instance* M2SceneNode, const CM2_Skin* M2Model, bool OpaqueDraw, UINT InstancesCnt = UINT32_MAX);

	// IRenderPassPipelined
	virtual std::shared_ptr<IRenderPassPipelined> CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport* Viewport) override;

    // IVisitor
    virtual bool Visit(const ISceneNode3D* node) override;
	virtual bool Visit(const IModel* Model) override;

protected:
	std::shared_ptr<ISettingT<float>> m_ADT_MDX_Distance;
	bool m_OpaqueDraw;

	const CM2_Base_Instance* m_CurrentM2Model;

	IShaderParameter* m_ShaderM2GeometryParameter;
	IShaderParameter* m_ShaderM2GeometryBonesParameter;
};


#if 0

class ZN_API CRenderPass_M2_Instanced
	: public CRenderPass_M2
{
public:
	CRenderPass_M2_Instanced(IRenderDevice& RenderDevice, const std::shared_ptr<BuildRenderListPassTemplated<CM2_Base_Instance>>& List, std::shared_ptr<IScene> scene, bool OpaqueDraw);
	virtual ~CRenderPass_M2_Instanced();

	void Render(RenderEventArgs& e) override;

	// IRenderPassPipelined
	virtual std::shared_ptr<IRenderPassPipelined> CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport* Viewport) override;

	// IVisitor
	bool Visit(const ISceneNode3D* node) override final;
	bool Visit(const IModel* Model) override final;

private:
	const std::shared_ptr<BuildRenderListPassTemplated<CM2_Base_Instance>> m_RenderListPass;
	IShaderParameter* m_ShaderInstancesBufferParameter;
	std::shared_ptr<IStructuredBuffer> m_InstancesBuffer;
	
};

#endif
