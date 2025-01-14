#include "stdafx.h"

// General
#include "M2_Base_Instance.h"

// Additional
#include "M2_ColliderComponent.h"

CM2_Base_Instance::CM2_Base_Instance(IScene& Scene, CWorldClient& WorldClient, const std::shared_ptr<CM2>& M2Object)
	: CSceneNode(Scene)
	, CLoadableObject(M2Object)
	, m_WorldClient(WorldClient)
	, m_M2(M2Object)
	, m_AttachmentType(EM2_AttachmentPoint::NotAttached)
	, m_Color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
	, m_Alpha(1.0f)
{

	dynamic_cast<IObjectPrivate*>(this)->SetGUID(Guid(ObjectTypes::otSceneNode, cSceneNodeM2BaseInstance, 1u));

	SetUpdateEnabled(false);
}

CM2_Base_Instance::~CM2_Base_Instance()
{
	//GetBaseManager().GetManager<ILoader>()->AddToDeleteQueue(m_M2);
}



//
// CM2_Base_Instance
//
const CM2& CM2_Base_Instance::GetM2() const
{
	auto state = m_M2->GetState();
	if (state != ILoadable::ELoadableState::Loaded)
		throw CException("CM2_Base_Instance::GetM2: M2 object isn't loaded. State = '%d'.", state);
	return *m_M2;
}

void CM2_Base_Instance::Attach(EM2_AttachmentPoint AttachmentType)
{
	m_AttachmentType = AttachmentType;
}
void CM2_Base_Instance::Detach()
{
	m_AttachmentType = EM2_AttachmentPoint::NotAttached;
}
void CM2_Base_Instance::UpdateAttachPositionAfterSkeletonUpdate()
{
	if (m_AttachmentType == EM2_AttachmentPoint::NotAttached)
		return;

	UpdateLocalTransform();
}

std::shared_ptr<CM2CameraComponent> CM2_Base_Instance::CreateCameraComponent(uint16 CameraDirectIndex)
{
	if (GetState() != ILoadable::ELoadableState::Loaded)
		throw CException("CM2_Base_Instance not loaded.");

	auto m2Camera = GetM2().getMiscellaneous().getCameraDirect(CameraDirectIndex);
	return AddComponentT(MakeShared(CM2CameraComponent, *this, m2Camera));
}

// Mesh & textures provider
bool CM2_Base_Instance::IsMeshEnabled(uint32 _index) const
{
	return true;
}

const std::shared_ptr<ITexture>& CM2_Base_Instance::GetSpecialTexture(SM2_Texture::Type _type) const
{
	if (_type >= SM2_Texture::Type::COUNT)
		throw CException("CM2_Base_Instance::getSpecialTexture: Out of bounds.");
	return m_SpecialTextures[_type];
}
void CM2_Base_Instance::SetSpecialTexture(SM2_Texture::Type _type, const std::shared_ptr<ITexture>& _texture)
{
	//_ASSERT(_texture != nullptr);
	m_SpecialTextures[_type] = _texture;
}
bool CM2_Base_Instance::IsInstansingEnabled() const
{
	return false;
}




//
// ISceneNode
//
void CM2_Base_Instance::Initialize()
{
	__super::Initialize();

	SetCullStrategy(ECullStrategy::ByFrustrumAndDistance2D);
	SetCullDistance(GetBaseManager().GetManager<ISettings>()->GetGroup("WoWSettings")->GetPropertyT<float>("M2BaseRenderDistance")->Get());
	SetDebugDrawMode(false);
	SetDebugDrawColor(ColorRGBA(0.9f, 0.2f, 0.2f, 0.8f));
}

void CM2_Base_Instance::RegisterComponents()
{
	AddComponentT(GetBaseManager().GetManager<IObjectsFactory>()->GetClassFactoryCast<IComponentFactory>()->CreateComponentT<IModelComponent>(cSceneNodeModelsComponent, *this));
	m_AnimatorComponent = AddComponentT(MakeShared(CM2AnimatorComponent, *this));
}

void CM2_Base_Instance::Update(const UpdateEventArgs & e)
{
	if (GetState() != ILoadable::ELoadableState::Loaded)
		return;
}

void CM2_Base_Instance::Accept(IVisitor* visitor)
{
	__super::Accept(visitor);
}



//
// ILoadable
//
bool CM2_Base_Instance::Load()
{
	SetBounds(GetM2().GetBounds());

	if (GetM2().isAnimated())
		m_AnimatorComponent->LoadAnimations();

	if (GetM2().getSkeleton().hasBones())
		m_SkeletonComponent = AddComponentT(MakeShared(CM2SkeletonComponent, *this));

#ifdef USE_M2_PARTICLES
	m_ParticleComponent = AddComponentT(MakeShared(CM2ParticlesComponent, *this));
#endif

	//m_LightComponent = AddComponentT(MakeShared(CM2LightComponent, *this));

	UpdateLocalTransform();

	GetM2().CreateInsances(shared_from_this());

	return true;
}

bool CM2_Base_Instance::Delete()
{
	return false;
}



//
// Protected
//
glm::mat4 CM2_Base_Instance::CalculateLocalTransform() const
{
	if (m_AttachmentType != EM2_AttachmentPoint::NotAttached)
	{
		if (auto parent = GetParent())
		{
			auto parentM2Instance = std::dynamic_pointer_cast<CM2_Base_Instance>(parent);
			if (parentM2Instance != nullptr)
			{
				//throw CException("CM2_Base_Instance::CalculateLocalTransform: Parent is nullptr.");
				//_ASSERT(parentM2Instance != nullptr);

				uint16 boneIndex = parentM2Instance->GetM2().getMiscellaneous().getAttachment(m_AttachmentType)->GetBoneIndex();

				const auto& bone = parentM2Instance->getSkeletonComponent()->GetBone(boneIndex);
				glm::mat4 relMatrix = glm::translate(bone->GetPivotPoint());

				return bone->GetMatrix() * relMatrix;
			}
		}

		return __super::CalculateLocalTransform();
	}
	else
	{
		return __super::CalculateLocalTransform();
	}
}


