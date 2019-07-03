#include "stdafx.h"

// General
#include "M2_Base_Instance.h"

// Additional
#include "M2_TransformComponent.h"
#include "M2_ColliderComponent.h"

CM2_Base_Instance::CM2_Base_Instance(std::string _m2Name) :
	m_M2(nullptr),
	m_M2Name(_m2Name),
	m_Attached(nullptr),
	m_Animator(nullptr),
	m_NeedRecalcAnimation(true),
	m_Color(vec4(1.0f, 1.0f, 1.0f, 1.0f)),
	m_Alpha(1.0f),
	m_QualitySettings(GetSettingsGroup<CGroupQuality>())
{
	for (uint8 i = 0; i < SM2_Texture::Type::COUNT; i++)
	{
		m_SpecialTextures[i] = _RenderDevice->GetDefaultTexture();
	}
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
	m_M2->CreateInsances(std::static_pointer_cast<CM2_Base_Instance, SceneNode3D>(shared_from_this()));
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
	std::shared_ptr<Texture> texture = _RenderDevice->CreateTexture2D(_textureName);
	setSpecialTexture(_type, texture);
}
void CM2_Base_Instance::setSpecialTexture(SM2_Texture::Type _type, std::shared_ptr<Texture> _texture)
{
	if (_texture != nullptr)
	{
		m_SpecialTextures[_type] = _texture;
	}
}
std::shared_ptr<Texture> CM2_Base_Instance::getSpecialTexture(SM2_Texture::Type _type) const
{
	_ASSERT(_type < SM2_Texture::Type::COUNT);
	return m_SpecialTextures[_type];
}

bool CM2_Base_Instance::Load()
{
	std::shared_ptr<M2> m2 = GetManager<IM2Manager>()->Add(m_M2Name);
	if (m2)
	{
		setM2(m2);



		CreateInstances();
		return true;
	}

	return false;
}

bool CM2_Base_Instance::Delete()
{
	return false;
}


//
//	m_M2->update(_time, _dTime);
//

bool CM2_Base_Instance::Accept(std::shared_ptr<IVisitor> visitor)
{
	std::shared_ptr<AbstractPass> visitorAsBasePass = std::dynamic_pointer_cast<AbstractPass>(visitor);
	const Camera* camera = visitorAsBasePass->GetRenderEventArgs()->Camera;

	float distToCamera2D = (camera->GetTranslation() - GetComponent<CColliderComponent3D>()->GetBounds().getCenter()).length() - GetComponent<CColliderComponent3D>()->GetBounds().getRadius();
	if (distToCamera2D > m_QualitySettings.ADT_MDX_Distance)
	{
		return false;
	}

	// Check frustrum
	if (!GetComponent<CColliderComponent3D>()->CheckFrustum(camera))
	{
		return false;
	}

	if (m_Attached != nullptr)
	{
        GetComponent<CTransformComponent3D>()->ForceRecalculateLocalTransform();
	}

	if (m_M2->isAnimated())
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
		m_M2->calc(m_Animator->getSequenceIndex(), m_Animator->getCurrentTime(), static_cast<uint32>(visitorAsBasePass->GetRenderEventArgs()->TotalTime), camera->GetViewMatrix(), GetComponent<CTransformComponent3D>()->GetWorldTransfom());
		//	m_NeedRecalcAnimation = true;
		//}
		//}
	}

	// SceneNode3D
	return SceneNode3D::Accept(visitor);
}

void CM2_Base_Instance::InitAnimator()
{
	// Create animator
	if (m_M2->isAnimated())
	{
		m_Animator = std::make_shared<CM2_Animator>(m_M2);
	}
}

const CGroupQuality& CM2_Base_Instance::GetGroupQuality() const
{
    return m_QualitySettings;
}

void CM2_Base_Instance::RegisterComponents()
{
    SetTransformComponent(AddComponent(std::make_shared<CM2_TransformComponent>(shared_from_this())));
    AddComponent(std::make_shared<CMeshComponent3D>(shared_from_this()));
    SetColliderComponent(AddComponent(std::make_shared<CM2_ColliderComponent>(shared_from_this())));
    AddComponent(std::make_shared<CLightComponent3D>(shared_from_this()));
}


