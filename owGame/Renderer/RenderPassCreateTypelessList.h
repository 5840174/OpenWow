#pragma once

class ZN_API CRenderPassCreateTypelessList
	: public ScenePass
	, public IRenderPassCreateTypelessList
{
public:
	CRenderPassCreateTypelessList(IRenderDevice& RenderDevice, IScene& Scene);
	virtual ~CRenderPassCreateTypelessList();

	const std::vector<SNodeElement>& GetNodesList() const override;
	const std::vector<SModelElement>& GetModelsList() const override;
	const std::vector<SGeometryElement>& GetGeometryList() const override;
	const std::vector<SLightElement>& GetLightList() const override;

	// IRenderPass
	void PreRender(RenderEventArgs& e) override;
	void Render(RenderEventArgs& e) override;

	// IVisitor
	EVisitResult Visit(const std::shared_ptr<ISceneNode>& SceneNode) override;
	EVisitResult Visit(const std::shared_ptr<IModel>& Model) override;
	EVisitResult Visit(const std::shared_ptr<IGeometry>& Geometry, const std::shared_ptr<IMaterial>& Material, SGeometryDrawArgs GeometryDrawArgs = SGeometryDrawArgs()) override;
	EVisitResult Visit(const std::shared_ptr<ILight>& light) override;

private:
	std::vector<SNodeElement> m_NodesList;
	//std::vector<SModelElement> m_ModelsList;
	//std::vector<SGeometryElement> m_GeometryList;
	//std::vector<SLightElement> m_LightList;
};
