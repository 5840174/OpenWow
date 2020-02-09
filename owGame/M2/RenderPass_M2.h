#pragma once

#include "M2/M2_Base_Instance.h"

class ZN_API CRenderPass_M2 
	: public Base3DPass
{
public:
	CRenderPass_M2(IRenderDevice& RenderDevice, std::shared_ptr<IScene> scene);
	virtual ~CRenderPass_M2();

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport* Viewport) override final;

    // IVisitor
    bool Visit(const ISceneNode3D* node) override final;
	bool Visit(const IModel* Model) override final;
	
private:
	const CM2_Base_Instance* m_CurrentM2Model;
};