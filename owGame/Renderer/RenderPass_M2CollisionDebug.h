#pragma once

#ifdef USE_WMO_MODELS

class ZN_API CRenderPass_M2CollisionDebug
	: public Base3DPass
{
public:
	CRenderPass_M2CollisionDebug(IScene& Scene);
	virtual ~CRenderPass_M2CollisionDebug();
	
	std::shared_ptr<MaterialDebug> GetMaterial() const { return m_MaterialDebug; }

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget) override final;

	// IVisitor
	EVisitResult Visit(const std::shared_ptr<ISceneNode>& node) override final;
	EVisitResult Visit(const std::shared_ptr<IGeometry>& Geometry, const std::shared_ptr<IMaterial>& Material, SGeometryDrawArgs GeometryDrawArgs) override final;

private:
	std::shared_ptr<MaterialDebug> m_MaterialDebug;
};

#endif 
