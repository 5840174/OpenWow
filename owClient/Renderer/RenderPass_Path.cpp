#include "stdafx.h"

// General
#include "RenderPass_Path.h"

// Additional
#include "World/World.h"
#include "World/Objects/WoWGameObjectMOTransport.h"

CRenderPass_Path::CRenderPass_Path(IRenderDevice& RenderDevice, CWoWWorld& WoWWorld)
	: RenderPassPipelined(RenderDevice)
	, m_WoWWorld(WoWWorld)
{
	m_PerObjectConstantBuffer = GetRenderDevice().GetObjectsFactory().CreateConstantBuffer(PerObject());
}

CRenderPass_Path::~CRenderPass_Path()
{}



//
// IRenderPass
//
void CRenderPass_Path::Render(RenderEventArgs & e)
{
	m_WoWWorld.Accept(this);
}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_Path::ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget)
{
	__super::ConfigurePipeline(RenderTarget);

	m_PointBox = GetRenderDevice().GetPrimitivesFactory().CreateCube();

	std::shared_ptr<IShader> vertexShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::VertexShader, "3D/Debug.hlsl", "VS_main");
	vertexShader->LoadInputLayoutFromReflector();

	std::shared_ptr<IShader> pixelShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::PixelShader, "3D/Debug.hlsl", "PS_main");
	
	//m_ShaderInstancesBufferParameter = &vertexShader->GetShaderParameterByName("Instances");
	//_ASSERT(m_ShaderInstancesBufferParameter->IsValid());

	m_MaterialDebug = MakeShared(MaterialDebug, GetRenderDevice());

	// PIPELINES
	GetPipeline().GetBlendState()->SetBlendMode(disableBlending);
	GetPipeline().GetDepthStencilState()->SetDepthMode(enableDepthWrites);
	GetPipeline().GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::None);
	GetPipeline().GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Solid, IRasterizerState::FillMode::Solid);
	GetPipeline().SetRenderTarget(RenderTarget);
	GetPipeline().SetShader(vertexShader);
	GetPipeline().SetShader(pixelShader);

	return shared_from_this();
}

EVisitResult CRenderPass_Path::VisitWoW(const std::shared_ptr<CWoWWorldObject>& WoWWorldObject)
{
	auto woWGameObjectMOTransport = std::dynamic_pointer_cast<WoWGameObjectMOTransport>(WoWWorldObject);
	if (woWGameObjectMOTransport == nullptr)
		return EVisitResult::AllowVisitChilds;

	const auto& pathID = woWGameObjectMOTransport->GetPathID();
	if (pathID == 0)
		return EVisitResult::AllowVisitChilds;

	const auto& wowWorld = woWGameObjectMOTransport->GetWoWWorld();

	uint32 mapID = UINT32_MAX;
	if (auto map = wowWorld.GetMap())
		mapID = map->GetMapID();

	const auto& pathRecords = woWGameObjectMOTransport->GetWoWWorld().GetTaxiStorage().GetPathNodes(pathID);
	std::vector<glm::vec3> pointsXYZ;
	std::for_each(pathRecords.begin(), pathRecords.end(), [&pointsXYZ, mapID](const CTaxiStorage::STaxiPathNode& Point) {
		if (mapID == Point.MapID)
			pointsXYZ.push_back(fromGameToReal(Point.Position));
	});

	if (pointsXYZ.size() < 2)
		return EVisitResult::AllowVisitChilds;

	BindPerObjectData(PerObject());

	m_WaypointGeometry = GetRenderDevice().GetPrimitivesFactory().CreateLines(pointsXYZ);

	m_MaterialDebug->Bind(GetPipeline().GetPixelShaderPtr());
	m_WaypointGeometry->Render(GetPipeline().GetVertexShaderPtr());
	m_MaterialDebug->Unbind(GetPipeline().GetPixelShaderPtr());

	return EVisitResult::AllowVisitChilds;
}



//
// Protected
//
void CRenderPass_Path::BindPerObjectData(const PerObject& PerObject)
{
	m_PerObjectConstantBuffer->Set(PerObject);

	for (const auto& shaderIt : GetPipeline().GetShaders())
	{
		if (auto* perObjectParam = shaderIt.second->GetShaderParameterByName("PerObject"))
		{
			perObjectParam->SetConstantBuffer(m_PerObjectConstantBuffer);
			perObjectParam->Bind();
		}
	}
}