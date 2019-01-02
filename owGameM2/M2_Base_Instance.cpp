#include "stdafx.h"

// General
#include "M2_Base_Instance.h"

CM2_Base_Instance::CM2_Base_Instance(std::shared_ptr<M2> _m2Object) :
	m_M2(nullptr),
	m_Attached(nullptr),
	m_Animator(nullptr),
	m_NeedRecalcAnimation(true),
	m_Time(0.0f),
	m_DTime(0.0f),
	m_Color(vec4(1.0f, 1.0f, 1.0f, 1.0f)),
	m_Alpha(1.0f)
{
	for (uint8 i = 0; i < SM2_Texture::Type::COUNT; i++)
	{
		m_SpecialTextures[i] = _RenderDevice->GetDefaultTexture();
	}

	if (_m2Object != nullptr)
	{
		setM2(_m2Object);
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

void CM2_Base_Instance::Load()
{
	m_M2->CreateInsances(std::static_pointer_cast<CM2_Base_Instance, SceneNode>(shared_from_this()));
}

void CM2_Base_Instance::Attach(const CM2_Part_Attachment* _attachment)
{
	assert1(_attachment != nullptr);
	m_Attached = _attachment;
}
void CM2_Base_Instance::Detach()
{
	m_Attached = nullptr;
}

void CM2_Base_Instance::setM2(std::shared_ptr<M2> _model)
{
	assert1(m_M2 == nullptr);
	assert1(_model != nullptr);
	m_M2 = _model;

	InitLocal();
	CalculateLocalTransform();
}

// Mesh & textures provider
bool CM2_Base_Instance::isMeshEnabled(uint32 _index) const
{
	return true;
}
void CM2_Base_Instance::setSpecialTexture(SM2_Texture::Type _type, cstring _textureName)
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
	assert1(_type < SM2_Texture::Type::COUNT);
	return m_SpecialTextures[_type];
}



// IUpdatable
void CM2_Base_Instance::Update(double _time, double _dTime)
{
	m_Time = _time;
	m_DTime = _dTime;
	
	m_M2->update(_time, _dTime);
}

void CM2_Base_Instance::Accept(IVisitor& visitor)
{
	const BasePass& visitorAsBasePass = reinterpret_cast<BasePass&>(visitor);
	const Camera& camera = *(visitorAsBasePass.GetRenderEventArgs().Camera);

	//float distToCamera2D = (camera.GetTranslation() - getBounds().getCenter()).length() - getBounds().getRadius();
	//if (distToCamera2D > m_QualitySettings.ADT_MCNK_Distance)
	//{
	//	return;
	//}

	// Check frustrum
	if (!checkFrustum(camera))
	{
		return;
	}

	/*if (m_Attached != nullptr)
	{
		CalculateLocalTransform();
	}

	if (m_M2->isAnimated())
	{
		m_Animator->Update(m_Time, m_DTime);

		//if (m_Object->isBillboard())
		//{
		//m_Object->calc(m_Animator->getAnimID(), m_Animator->getCurrentTime(_time), _time);
		//}
		//else
		//{
		//if (!m_NeedRecalcAnimation)
		//{
		m_M2->calc(m_Animator->getSequenceIndex(), GetWorldTransfom(), m_Animator->getCurrentTime(), static_cast<uint32>(m_Time));
		//	m_NeedRecalcAnimation = true;
		//}
		//}

		//m_M2->Render(thisGetWorldTransfom(), m_MeshProvider, m_DoodadColor, m_Animator->getSequenceIndex(), m_Animator->getCurrentTime(), static_cast<uint32>(m_Time));
	}*/

	//_Render->DrawBoundingBox(getBounds());

	//m_M2->Render(this/*GetWorldTransfom(), m_MeshProvider, m_DoodadColor, 0, 0, static_cast<uint32>(m_Time)*/);

	// SceneNode
	SceneNode::Accept(visitor);
}


//-----------------
// ISceneNode
//-----------------

void CM2_Base_Instance::InitLocal()
{
	// Create animator
	if (m_M2->isAnimated())
	{
		m_Animator = std::make_shared<CM2_Animator>(m_M2);
		//_Bindings->RegisterUpdatableObject(this);
	}
}

void CM2_Base_Instance::CalculateLocalTransform(bool _isRotationQuat)
{
	if (m_Attached != nullptr)
	{
		std::shared_ptr<const CM2_Part_Bone> bone = m_Attached->getBone().lock();
		assert1(bone != nullptr);

		mat4 relMatrix;
		relMatrix = glm::translate(relMatrix, bone->getPivot());

		mat4 absMatrix;
		absMatrix = GetParentWorldTransform() * bone->getTransformMatrix() * relMatrix;
		SetWorldTransform(absMatrix);

		BoundingBox bbox = m_M2->m_Bounds;
		bbox.transform(GetWorldTransfom());
		setBounds(bbox);

		return;
	}

	SceneNode::CalculateLocalTransform(_isRotationQuat);

	BoundingBox bbox = m_M2->m_Bounds;
	bbox.transform(GetWorldTransfom());
	setBounds(bbox);
}
