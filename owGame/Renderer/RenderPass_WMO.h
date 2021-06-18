#pragma once

#ifdef USE_WMO_MODELS

class ZN_API CRenderPass_WMO 
	: public Base3DPass
{
public:
	CRenderPass_WMO(IScene& Scene);
	virtual ~CRenderPass_WMO();

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget) override final;

    // IVisitor
    EVisitResult Visit(const std::shared_ptr<ISceneNode>& node) override final;
	EVisitResult Visit(const std::shared_ptr<IGeometry>& Geometry, const std::shared_ptr<IMaterial>& Material, SGeometryDrawArgs GeometryDrawArgs) override final;

private:
	std::shared_ptr<IPropertiesGroup> m_WoWSettings;
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
