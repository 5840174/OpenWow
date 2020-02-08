#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Base_Instance.h"
#include "M2_Skin_Builder.h"

// General
#include "M2_Skin_Batch.h"

CM2_Skin_Batch::CM2_Skin_Batch(IBaseManager* BaseManager, IRenderDevice& RenderDevice, const M2& M2Model, const CM2_SkinSection& SkinSection)
	: ModelProxie(RenderDevice.GetObjectsFactory().CreateModel())
	, m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
	, m_M2Model(M2Model)
	, m_SkinSection(SkinSection)
{
	
}

CM2_Skin_Batch::~CM2_Skin_Batch()
{
}

bool CM2_Skin_Batch::Render(const RenderEventArgs& renderEventArgs) const
{
	/*
	const ISceneNode3D* sceneNode = dynamic_cast<const ISceneNode3D*>(renderEventArgs.Node);
	_ASSERT(sceneNode != nullptr);

	const CM2_Base_Instance* sceneNodeAsM2Instance = dynamic_cast<const CM2_Base_Instance*>(sceneNode);
	_ASSERT(sceneNodeAsM2Instance != nullptr);

	const ICameraComponent3D* camera = renderEventArgs.Camera;
	_ASSERT(camera != nullptr);

	

	uint32 meshPartID = skinProto.meshPartID;
	if (!sceneNodeAsM2Instance->isMeshEnabled(meshPartID))
	{
		return false;
	}*/

	const SM2_SkinSection& skinProto = m_SkinSection.getProto();

	//m_TestMaterial->SetBlendMode(static_cast<uint32>(m_Material->getBlendMode()));

	std::shared_ptr<CM2_Comp_Skeleton> skeleton = m_M2Model.getSkeleton();

	bool isAnimated = skeleton->hasBones() && m_M2Model.m_IsAnimated;
	m_TestMaterial->SetAnimated(isAnimated ? 1 : 0);
	if (isAnimated)
	{
		m_TestMaterial->SetMaxInfluences(skinProto.boneInfluences);

		//for (uint16 i = skinProto.bonesStartIndex; i < skinProto.bonesStartIndex + skinProto.boneCount; i++)
		//	skeleton->getBoneLookup(i)->SetNeedCalculate();

		//for (uint16 i = skinProto.bonesStartIndex; i < skinProto.bonesStartIndex + skinProto.boneCount; i++)
		//	skeleton->getBoneLookup(i)->calcMatrix(sceneNodeAsM2Instance->getAnimator()->getSequenceIndex(), sceneNodeAsM2Instance->getAnimator()->getCurrentTime(), static_cast<uint32>(renderEventArgs.TotalTime));

		//for (uint16 i = skinProto.bonesStartIndex; i < skinProto.bonesStartIndex + skinProto.boneCount; i++)
		//	skeleton->getBoneLookup(i)->calcBillboard(camera->GetViewMatrix(), sceneNodeAsM2Instance->GetWorldTransfom());

		std::vector<mat4> bones;
		for (uint16 i = skinProto.bonesStartIndex; i < skinProto.bonesStartIndex + skinProto.boneCount; i++)
		{
			_ASSERT(skeleton->isLookupBoneCorrect(i));
			bones.push_back(skeleton->getBoneLookup(i)->getTransformMatrix());
		}

		m_TestMaterial->SetBones(bones);
	}

	// Shader
	m_TestMaterial->SetNewShader(newShader);

	// Model color
	bool isColorEnable = (m_Color != nullptr);
	m_TestMaterial->SetColorEnable(isColorEnable);
	if (isColorEnable)
	{
		m_TestMaterial->SetColor(m_Color->getValue());
	}

	// Textures
	/*for (uint32 i = 0; i < m_Textures.size(); i++)
	{
        std::shared_ptr<const CM2_Part_Texture> m2Texture = m_Textures[i].lock();

		m_TestMaterial->SetTexture(i, m2Texture->GetResultTexture(sceneNodeAsM2Instance));
        m_TestMaterial->GetSampler(i)->SetWrapMode(m2Texture->GetTextureWrapX(), m2Texture->GetTextureWrapY());
	}*/

	// Texture alpha
	bool isTextureWeightEnable = (m_TextureWeight != nullptr);
	m_TestMaterial->SetTextureWeightEnable(isTextureWeightEnable);
	if (isTextureWeightEnable)
	{
		m_TestMaterial->SetTextureWeight(m_TextureWeight->getValue());
	}

	// Texture transform
	bool isTextureTransformEnable = (m_TextureTransform != nullptr);
	m_TestMaterial->SetTextureAnimEnable(isTextureTransformEnable);
	if (isTextureTransformEnable)
	{
		m_TestMaterial->SetTextureAnimMatrix(m_TextureTransform->getValue());
	}

	const ShaderMap& shaders = renderEventArgs.PipelineState->GetShaders();
	const IShader* vertexShader = shaders.at(EShaderType::VertexShader).get();

	for (const auto& connection : GetConnections())
	{
		SGeometryDrawArgs geometryDrawArgs = connection.GeometryDrawArgs;
		geometryDrawArgs.IndexCnt = skinProto.indexCount;
		geometryDrawArgs.VertexCnt = skinProto.vertexCount;

		connection.Material->Bind(shaders);
		connection.Geometry->Render(renderEventArgs, vertexShader, connection.GeometryDrawArgs);
		connection.Material->Unbind(shaders);
	}

	return true;
}

void CM2_Skin_Batch::Accept(IVisitor * visitor)
{
	visitor->Visit(this);

	// Don't visit geometry
}
