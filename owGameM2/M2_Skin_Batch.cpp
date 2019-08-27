#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Base_Instance.h"
#include "M2_Skin_Builder.h"

// General
#include "M2_Skin_Batch.h"

CM2_Skin_Batch::CM2_Skin_Batch(const std::weak_ptr<const M2> _parentM2, std::shared_ptr<IMesh> _mesh) :
	MeshWrapper(_mesh),
	m_ParentM2(_parentM2),
	m_QualitySettings(GetSettingsGroup<CGroupQuality>())
{

}

void CM2_Skin_Batch::PostInit()
{
	m_TestMaterial = std::make_shared<M2_Material>(m_Textures);
	m_TestMaterial->SetWrapper(m_TestMaterial);

	SetMaterial(m_TestMaterial);
}

bool CM2_Skin_Batch::Render(const RenderEventArgs* renderEventArgs, const ConstantBuffer* perObject, UINT indexStartLocation, UINT indexCnt, UINT vertexStartLocation, UINT vertexCnt)
{
	const SceneNode3D* sceneNode = dynamic_cast<const SceneNode3D*>(renderEventArgs->Node);
	_ASSERT(sceneNode != nullptr);

	const CM2_Base_Instance* sceneNodeAsM2Instance = dynamic_cast<const CM2_Base_Instance*>(sceneNode);
	_ASSERT(sceneNodeAsM2Instance != nullptr);

	const Camera* camera = renderEventArgs->Camera;
	_ASSERT(camera != nullptr);

	const SM2_SkinSection& proto = m_SkinSection->getProto();

	uint32 meshPartID = proto.meshPartID;
	if (!sceneNodeAsM2Instance->isMeshEnabled(meshPartID))
	{
		return false;
	}

	m_TestMaterial->SetBlendMode(static_cast<uint32>(m_Material->getBlendMode()));

	std::shared_ptr<CM2_Comp_Skeleton> skeleton = m_ParentM2.lock()->getSkeleton();

	bool isAnimated = skeleton->hasBones() && m_ParentM2.lock()->m_IsAnimated;
	m_TestMaterial->SetAnimated(isAnimated ? 1 : 0);
	if (isAnimated)
	{
		m_TestMaterial->SetMaxInfluences(proto.boneInfluences);

		//for (uint16 i = proto.bonesStartIndex; i < proto.bonesStartIndex + proto.boneCount; i++)
		//	skeleton->getBoneLookup(i)->SetNeedCalculate();

		//for (uint16 i = proto.bonesStartIndex; i < proto.bonesStartIndex + proto.boneCount; i++)
		//	skeleton->getBoneLookup(i)->calcMatrix(sceneNodeAsM2Instance->getAnimator()->getSequenceIndex(), sceneNodeAsM2Instance->getAnimator()->getCurrentTime(), static_cast<uint32>(renderEventArgs.TotalTime));

		//for (uint16 i = proto.bonesStartIndex; i < proto.bonesStartIndex + proto.boneCount; i++)
		//	skeleton->getBoneLookup(i)->calcBillboard(camera->GetViewMatrix(), sceneNodeAsM2Instance->GetWorldTransfom());

		std::vector<mat4> bones;
		for (uint16 i = proto.bonesStartIndex; i < proto.bonesStartIndex + proto.boneCount; i++)
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
	for (uint32 i = 0; i < m_Textures.size(); i++)
	{
        std::shared_ptr<const CM2_Part_Texture> m2Texture = m_Textures[i].lock();

		m_TestMaterial->SetTexture(i, m2Texture->GetResultTexture(sceneNodeAsM2Instance));
        m_TestMaterial->GetSampler(i)->SetWrapMode(m2Texture->GetTextureWrapX(), m2Texture->GetTextureWrapY());
	}

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

	
	return MeshWrapper::Render(renderEventArgs, perObject, 0, proto.indexCount, 0, proto.vertexCount);
}
