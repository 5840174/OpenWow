#pragma once

class ZN_API CRenderPassPipelinedProcessTypelessList
	: public RenderPassPipelined
	, public IVisitor
{
public:
	CRenderPassPipelinedProcessTypelessList(IRenderDevice& RenderDevice, const std::shared_ptr<IRenderPassCreateTypelessList>& CreateTypelessList);
	virtual ~CRenderPassPipelinedProcessTypelessList();

	// IRenderPassPipelined
	void Render(RenderEventArgs& e);

	// IVisitor
	virtual EVisitResult Visit(const std::shared_ptr<ISceneNode>& SceneNode) override;
	virtual EVisitResult Visit(const std::shared_ptr<IUIControl>& UIControl) override;
	virtual EVisitResult Visit(const std::shared_ptr<IModel>& Model) override;
	virtual EVisitResult Visit(const std::shared_ptr<IGeometry>& Geometry, const std::shared_ptr<IMaterial>& Material, SGeometryDrawArgs GeometryDrawArgs) override;
	virtual EVisitResult Visit(const std::shared_ptr<ILight>& light) override;
	virtual EVisitResult Visit(const std::shared_ptr<IParticleSystem>& ParticleSystem) override;
	virtual EVisitResult Visit(const std::shared_ptr<ITerrain>& Terrain) override;

protected:
	void BindPerObjectData(const PerObject& PerObject);
	const std::shared_ptr<IRenderPassCreateTypelessList>& GetSceneNodeListPass() const;

protected:
	std::shared_ptr<IRenderPassCreateTypelessList> m_CreateTypelessList;

private:
	std::shared_ptr<IConstantBuffer> m_PerObjectConstantBuffer;

	IShaderParameter* m_PerObjectParameters[(size_t)EShaderType::ComputeShader + 1ull];
};