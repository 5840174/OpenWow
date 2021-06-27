#pragma once

#ifdef USE_WMO_MODELS

#include "RenderPassProcessTypelessList.h"

#include "WMO/WMO_Group_Instance.h"
#include "WMO/WMO_Group_Part_Batch.h"

class ZN_API CRenderPass_WMOList 
	: public CRenderPassProcessTypelessList
{
public:
	CRenderPass_WMOList(IRenderDevice& RenderDevice, const std::shared_ptr<IRenderPassCreateTypelessList>& CreateTypelessList);
	virtual ~CRenderPass_WMOList();

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget) override final;

    // IVisitor
    EVisitResult Visit(const std::shared_ptr<ISceneNode>& SceneNode) override final;
	EVisitResult Visit(const std::shared_ptr<IModel>& Model) override final;
	EVisitResult Visit(const std::shared_ptr<IGeometry>& Geometry, const std::shared_ptr<IMaterial>& Material, SGeometryDrawArgs GeometryDrawArgs) override final;

private:
	std::shared_ptr<IPropertiesGroup> m_WoWSettings;
	const CWMO_Group_Instance * m_CurrentWMOGroupInstance;
	const WMO_Group_Part_Batch * m_CurrentWMOBatch;
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

#endif 
