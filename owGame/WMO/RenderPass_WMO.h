#pragma once

#include "WMO_Base_Instance.h"

class ZN_API CRenderPass_WMO 
	: public CBaseList3DPass
{
public:
	CRenderPass_WMO(IRenderDevice& RenderDevice, const std::shared_ptr<CSceneCreateTypedListsPass>& SceneNodeListPass);
	virtual ~CRenderPass_WMO();

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport* Viewport) override final;

    // IVisitor
    EVisitResult Visit(const ISceneNode3D* node) override final;

private:
	std::shared_ptr<ISettingGroup> m_WoWSettings;
};

#if 0

class ZN_API CRenderPass_WMO2 
	: public Base3DPass
{
public:
	CRenderPass_WMO2(IRenderDevice& RenderDevice, const std::shared_ptr<BuildRenderListPassTemplated<CWMO_Group_Instance>>& List, std::shared_ptr<IScene> scene);
	virtual ~CRenderPass_WMO2();

	// IRenderPass
	void Render(RenderEventArgs& e) override;

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport* Viewport) override final;

private:
	std::shared_ptr<ISettingGroup> m_WoWSettings;
	std::shared_ptr<BuildRenderListPassTemplated<CWMO_Group_Instance>> m_List;

	std::shared_ptr<IStructuredBuffer> strBuffer;
};

#endif
