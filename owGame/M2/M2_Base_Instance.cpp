#include "stdafx.h"

// General
#include "M2_Base_Instance.h"

// Additional
#include "M2_ColliderComponent.h"

CM2_Base_Instance::CM2_Base_Instance(const M2& M2Object) 
	: CLoadableObject(&M2Object)
	, m_M2(M2Object)
	, m_Attached(nullptr)
	, m_Animator(nullptr)
	, m_NeedRecalcAnimation(true)
	, m_Color(vec4(1.0f, 1.0f, 1.0f, 1.0f))
	, m_Alpha(1.0f)
{

}

CM2_Base_Instance::~CM2_Base_Instance()
{
	if (m_M2.isAnimated())
	{
		//_Bindings->UnregisterUpdatableObject(this);
	}
}

// CM2_Base_Instance

void CM2_Base_Instance::CreateInstances()
{
	m_M2.CreateInsances(shared_from_this());
}

bool CM2_Base_Instance::Load()
{
	InitAnimator();
	UpdateLocalTransform();
	CreateInstances();

	return true;
}

void CM2_Base_Instance::Attach(std::shared_ptr<CM2_Part_Attachment> _attachment)
{
	_ASSERT(_attachment != nullptr);
	m_Attached = _attachment;
}
void CM2_Base_Instance::Detach()
{
	m_Attached = nullptr;
}

std::shared_ptr<CM2_Part_Attachment> CM2_Base_Instance::GetAttachPoint() const
{
    return m_Attached;
}

// Mesh & textures provider
bool CM2_Base_Instance::isMeshEnabled(uint32 _index) const
{
	return true;
}
void CM2_Base_Instance::setSpecialTexture(SM2_Texture::Type _type, const std::string& _textureName)
{
	_ASSERT(FALSE);
	//std::shared_ptr<ITexture> texture = GetRenderDevice()->GetObjectsFactory().LoadTexture2D(_textureName);
	//setSpecialTexture(_type, texture);
}
void CM2_Base_Instance::setSpecialTexture(SM2_Texture::Type _type, std::shared_ptr<ITexture> _texture)
{
	if (_texture != nullptr)
	{
		m_SpecialTextures[_type] = _texture;
	}
}
std::shared_ptr<ITexture> CM2_Base_Instance::getSpecialTexture(SM2_Texture::Type _type) const
{
	_ASSERT(_type < SM2_Texture::Type::COUNT);
	return m_SpecialTextures[_type];
}


//
//	m_M2->update(_time, _dTime);
//

void CM2_Base_Instance::Initialize()
{
	GetComponent<CColliderComponent3D>()->SetBounds(getM2().GetBounds());
	GetComponent<CColliderComponent3D>()->SetDebugDrawMode(false);
}

void CM2_Base_Instance::Update(const UpdateEventArgs & e)
{
	if (GetState() != ILoadable::ELoadableState::Loaded)
		return;

	if (m_Attached != nullptr)
	{
		ForceRecalculateLocalTransform();
	}

	const_cast<M2&>(m_M2).update(e.TotalTime, e.DeltaTime);

	if (m_M2.isAnimated())
	{
		m_Animator->Update(e.TotalTime, e.DeltaTime);

		//if (m_Object->isBillboard())
		//{
		//m_Object->calc(m_Animator->getAnimID(), m_Animator->getCurrentTime(_time), _time);
		//}
		//else
		//{
		//if (!m_NeedRecalcAnimation)
		{
			//const_cast<M2&>(m_M2).calc(m_Animator->getSequenceIndex(), m_Animator->getCurrentTime(), e.TotalTime, e.Camera->GetViewMatrix(), GetWorldTransfom());
			//m_NeedRecalcAnimation = true;
		}
	}
}

void CM2_Base_Instance::Accept(IVisitor* visitor)
{
	SceneNode3D::Accept(visitor);
}

//
// CTransformComponent
//
void CM2_Base_Instance::UpdateLocalTransform()
{
	if (std::shared_ptr<CM2_Part_Attachment> attachPoint = GetAttachPoint())
	{
		std::shared_ptr<const CM2_Part_Bone> bone = attachPoint->getBone().lock();
		_ASSERT(bone != nullptr);

		glm::mat4 relMatrix;
		relMatrix = glm::translate(relMatrix, bone->getPivot());

		glm::mat4 absMatrix;
		absMatrix = GetParentWorldTransform() * bone->getTransformMatrix() * relMatrix;
		SetWorldTransform(absMatrix);
	}
	else
	{
		SceneNode3D::UpdateLocalTransform();
	}
}

void CM2_Base_Instance::InitAnimator()
{
	// Create animator
	if (m_M2.isAnimated())
	{
		m_Animator = std::make_shared<CM2_Animator>(GetBaseManager(), m_M2);
	}
}

void CM2_Base_Instance::RegisterComponents()
{
	SetMeshComponent(AddComponent(std::make_shared<CMeshComponent3D>(*this)));
    SetColliderComponent(AddComponent(std::make_shared<CM2_ColliderComponent>(*this)));
}
