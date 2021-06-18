#include "stdafx.h"

// General
#include "M2_Base_Instance.h"

// Additional
#include "M2_ColliderComponent.h"

CM2_Base_Instance::CM2_Base_Instance(IScene& Scene, const std::shared_ptr<CM2>& M2Object) 
	: CSceneNode(Scene)
	, CLoadableObject(M2Object)
	, m_M2(M2Object)
	, m_AttachmentType(M2_AttachmentType::NotAttached)
	, m_Color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
	, m_Alpha(1.0f)
	, m_Animator(nullptr)
{
}

CM2_Base_Instance::~CM2_Base_Instance()
{
	if (getM2().isAnimated())
	{
		//_Bindings->UnregisterUpdatableObject(this);
	}
}

// CM2_Base_Instance

void CM2_Base_Instance::CreateInstances()
{
	getM2().CreateInsances(shared_from_this());
}

bool CM2_Base_Instance::Load()
{
	if (getM2().isAnimated())
		m_Animator = std::make_shared<CM2_Animator>(GetBaseManager(), getM2());

	if (getM2().getSkeleton().hasBones())
	{
		m_SkeletonComponent = AddComponentT(std::make_shared<CM2SkeletonComponent3D>(*this));
	}

#ifdef USE_M2_PARTICLES
	m_ParticleComponent = AddComponentT(std::make_shared<CM2ParticlesComponent3D>(*this));
#endif

	UpdateLocalTransform();
	CreateInstances();

	return true;
}

bool CM2_Base_Instance::Delete()
{
	return false;
}

const CM2& CM2_Base_Instance::getM2() const
{
	_ASSERT(m_M2 != nullptr);
	return *m_M2;
}

void CM2_Base_Instance::Attach(M2_AttachmentType AttachmentType)
{
	m_AttachmentType = AttachmentType;
}
void CM2_Base_Instance::Detach()
{
	m_AttachmentType = M2_AttachmentType::NotAttached;
}

void CM2_Base_Instance::UpdateAttachPositionAfterSkeletonUpdate()
{
	if (m_AttachmentType == M2_AttachmentType::NotAttached)
		return;

	UpdateLocalTransform();
}

// Mesh & textures provider
bool CM2_Base_Instance::isMeshEnabled(uint32 _index) const
{
	return true;
}
void CM2_Base_Instance::setSpecialTexture(SM2_Texture::Type _type, const std::shared_ptr<ITexture>& _texture)
{
	//_ASSERT(_texture != nullptr);
	m_SpecialTextures[_type] = _texture;
}
const std::shared_ptr<ITexture>& CM2_Base_Instance::getSpecialTexture(SM2_Texture::Type _type) const
{
	_ASSERT(_type < SM2_Texture::Type::COUNT);
	return m_SpecialTextures[_type];
}


//
//	m_M2->update(_time, _dTime);
//

void CM2_Base_Instance::Initialize()
{
	__super::Initialize();

	GetComponentT<IColliderComponent>()->SetCullStrategy(IColliderComponent::ECullStrategy::ByFrustrumAndDistance);
	GetComponentT<IColliderComponent>()->SetCullDistance(GetBaseManager().GetManager<ISettings>()->GetGroup("WoWSettings")->GetPropertyT<float>("ADT_MDX_Distance")->Get());
	GetComponentT<IColliderComponent>()->SetBounds(getM2().GetBounds());
	GetComponentT<IColliderComponent>()->SetDebugDrawMode(false);
}

void CM2_Base_Instance::Update(const UpdateEventArgs & e)
{
	if (GetState() != ILoadable::ELoadableState::Loaded)
		return;

	if (GetComponentT<IColliderComponent>()->IsCulled(e.Camera))
		return;

	if (m_Animator)
		m_Animator->Update(e.TotalTime, e.DeltaTime);

#ifdef USE_M2_PARTICLES
	if (m_ParticleComponent)
		m_ParticleComponent->Update(e);
#endif
}

void CM2_Base_Instance::Accept(IVisitor* visitor)
{
	CSceneNode::Accept(visitor);
}



//
// Protected
//
glm::mat4 CM2_Base_Instance::CalculateLocalTransform() const
{
	if (m_AttachmentType != M2_AttachmentType::NotAttached)
	{
		if (auto parent = GetParent())
		{
			auto parentM2Instance = std::dynamic_pointer_cast<CM2_Base_Instance>(parent);
			_ASSERT(parentM2Instance != nullptr);

			uint16 boneIndex = parentM2Instance->getM2().getMiscellaneous().getAttachment(m_AttachmentType).GetBoneIndex();

			const auto& bone = parentM2Instance->getSkeletonComponent()->GetBone(boneIndex);
			glm::mat4 relMatrix = glm::translate(bone->GetPivotPoint());

			return bone->GetMatrix() * relMatrix;
		}

		return __super::CalculateLocalTransform();
	}
	else
	{
		return __super::CalculateLocalTransform();
	}
}

void CM2_Base_Instance::RegisterComponents()
{
	AddComponentT(GetBaseManager().GetManager<IObjectsFactory>()->GetClassFactoryCast<IComponentFactory>()->CreateComponentT<IModelComponent>(cSceneNodeModelsComponent, *this));
    AddComponentT(std::make_shared<CM2_ColliderComponent>(*this));
}
