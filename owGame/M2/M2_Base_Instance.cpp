#include "stdafx.h"

// General
#include "M2_Base_Instance.h"

// Additional
#include "M2_ColliderComponent.h"

CM2_Base_Instance::CM2_Base_Instance(std::string _m2Name) :
	m_M2(nullptr),
	m_M2Name(_m2Name),
	m_Attached(nullptr),
	m_Animator(nullptr),
	m_NeedRecalcAnimation(true),
	m_Color(vec4(1.0f, 1.0f, 1.0f, 1.0f)),
	m_Alpha(1.0f)
{

}

CM2_Base_Instance::~CM2_Base_Instance()
{
	if (m_M2->isAnimated())
	{
		//_Bindings->UnregisterUpdatableObject(this);
	}
}

// CM2_Base_Instance

void CM2_Base_Instance::CreateInstances()
{
	m_M2->CreateInsances(std::static_pointer_cast<CM2_Base_Instance>(shared_from_this()));
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

void CM2_Base_Instance::setM2(std::shared_ptr<M2> _model)
{
	_ASSERT(m_M2 == nullptr);
	_ASSERT(_model != nullptr);
	m_M2 = _model;

	InitAnimator();
	//UpdateLocalTransform();
}

// Mesh & textures provider
bool CM2_Base_Instance::isMeshEnabled(uint32 _index) const
{
	return true;
}
void CM2_Base_Instance::setSpecialTexture(SM2_Texture::Type _type, const std::string& _textureName)
{
	_ASSERT(false);
	//std::shared_ptr<ITexture> texture = GetBaseManager()->GetManager<IRenderDevice>()->CreateTexture2D(_textureName);
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
	for (uint8 i = 0; i < SM2_Texture::Type::COUNT; i++)
	{
		m_SpecialTextures[i] = nullptr; // TODO: GetBaseManager()->GetManager<IRenderDevice>()->GetDefaultTexture();
	}
}

void CM2_Base_Instance::Accept(IVisitor* visitor)
{
	//AbstractPass* visitorAsBasePass = dynamic_cast<AbstractPass*>(visitor);
	//const ICamera* camera = visitorAsBasePass->GetRenderEventArgs()->Camera;

	if (m_M2 == nullptr)
		return;

	//float distToCamera2D = (camera->GetTranslation() - GetComponent<IColliderComponent3D>()->GetBounds().getCenter()).length() - GetComponent<IColliderComponent3D>()->GetBounds().getRadius();
	//if (distToCamera2D > m_QualitySettings->ADT_MDX_Distance)
	//{
	//	return false;
	//}

	//if (!GetComponent<IColliderComponent3D>()->CheckFrustum(camera))
	//{
	//	return false;
	//}

	if (m_Attached != nullptr)
	{
		// TODO:
        //GetComponent<ITransformComponent3D>()->ForceRecalculateLocalTransform();
	}

	/*if (m_M2->isAnimated())
	{
		m_Animator->Update(visitorAsBasePass->GetRenderEventArgs()->TotalTime, visitorAsBasePass->GetRenderEventArgs()->ElapsedTime);

		//if (m_Object->isBillboard())
		//{
		//m_Object->calc(m_Animator->getAnimID(), m_Animator->getCurrentTime(_time), _time);
		//}
		//else
		//{
		//if (!m_NeedRecalcAnimation)
		//{
		m_M2->calc(m_Animator->getSequenceIndex(), m_Animator->getCurrentTime(), static_cast<uint32>(visitorAsBasePass->GetRenderEventArgs()->TotalTime), camera->GetViewMatrix(), GetWorldTransfom());
		//	m_NeedRecalcAnimation = true;
		//}
		//}
	}*/

	// SceneNode3D
	SceneNode3D::Accept(visitor);
}

//
// CTransformComponent
//
void CM2_Base_Instance::UpdateLocalTransform()
{
	std::shared_ptr<CM2_Part_Attachment> attachPoint = GetAttachPoint();

	if (attachPoint)
	{
		std::shared_ptr<const CM2_Part_Bone> bone = attachPoint->getBone().lock();
		_ASSERT(bone != nullptr);

		mat4 relMatrix;
		relMatrix = glm::translate(relMatrix, bone->getPivot());

		mat4 absMatrix;
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
	if (m_M2->isAnimated())
	{
		m_Animator = std::make_shared<CM2_Animator>(*m_M2);
	}
}

void CM2_Base_Instance::RegisterComponents()
{
	SetMeshComponent(AddComponent(std::make_shared<CMeshComponent3D>(*this)));
    SetColliderComponent(AddComponent(std::make_shared<CM2_ColliderComponent>(*this)));
}










//
// // ILoadableObject
//
bool CM2_Base_Instance::Load()
{
	std::shared_ptr<M2> m2 = GetBaseManager()->GetManager<IM2Manager>()->Add(GetBaseManager()->GetApplication().GetRenderDevice(), m_M2Name);
	if (m2)
	{
		setM2(m2);

		CreateInstances();
		return true;
	}

	return false;
}
