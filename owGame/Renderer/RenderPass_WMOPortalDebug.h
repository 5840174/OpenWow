#pragma once

#ifdef USE_WMO_MODELS

class ZN_API CRenderPass_WMOPortalDebug
	: public Base3DPass
{
public:
	CRenderPass_WMOPortalDebug(IScene& Scene);
	virtual ~CRenderPass_WMOPortalDebug();

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget) override final;

	// IVisitor
	EVisitResult Visit(const std::shared_ptr<ISceneNode>& node) override final;
	EVisitResult Visit(const std::shared_ptr<IGeometry>& Geometry, const std::shared_ptr<IMaterial>& Material, SGeometryDrawArgs GeometryDrawArgs) override final;

private:
	std::shared_ptr<IPropertiesGroup> m_WoWSettings;
	std::shared_ptr<MaterialDebug>     m_MaterialDebug;
	std::shared_ptr<IGeometry>         m_WaypointGeometry;
};

#endif 
