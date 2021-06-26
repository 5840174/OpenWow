#pragma once

#include "../World/Objects/WoWWorldObject.h"

// FORWARD BEGIN
class CWoWWorldObject;
// FORWARD END

ZN_INTERFACE ZN_API IWoWVisitor
{
	virtual ~IWoWVisitor() {}

	virtual EVisitResult VisitWoW(const std::shared_ptr<CWoWWorldObject>& WoWWorldObject) = 0;
};

class CRenderPass_Path
	: public RenderPassPipelined
	, public IWoWVisitor
{
public:
	CRenderPass_Path(IRenderDevice& RenderDevice, CWoWWorld& WoWWorld);
	virtual ~CRenderPass_Path();

	// IRenderPass
	void Render(RenderEventArgs& e) override;

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget) override final;

	// IVisitor
	EVisitResult VisitWoW(const std::shared_ptr<CWoWWorldObject>& WoWWorldObject) override final;

	// Protected
	void BindPerObjectData(const PerObject& PerObject);

protected:
	std::shared_ptr<MaterialDebug>     m_MaterialDebug;
	std::shared_ptr<IGeometry>         m_WaypointGeometry;

	std::shared_ptr<IGeometry>         m_PointBox;

	std::shared_ptr<IConstantBuffer>   m_PerObjectConstantBuffer;
	//IShaderParameter*                m_PerObjectParameter;

	IShaderParameter*                  m_ShaderInstancesBufferParameter;
	std::shared_ptr<IStructuredBuffer> m_InstancesBuffer;
	size_t							   m_InstancesCnt;

	CWoWWorld& m_WoWWorld;
};
