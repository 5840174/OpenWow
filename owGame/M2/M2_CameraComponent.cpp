#include "stdafx.h"

// General
#include "M2_CameraComponent.h"

// Include
#include "M2.h"
#include "M2_Base_Instance.h"

CM2CameraComponent::CM2CameraComponent(const CM2_Base_Instance& OwnerNode, const std::shared_ptr<const CM2_Part_Camera>& M2Camera)
	: CSceneNodeComponentBase(OwnerNode)
	, m_M2Camera(M2Camera)
{
	GetProperties()->SetName("M2CameraComponent");
}

CM2CameraComponent::~CM2CameraComponent()
{}

void CM2CameraComponent::DoMoveFront(float Value)
{}

void CM2CameraComponent::DoMoveBack(float Value)
{}

void CM2CameraComponent::DoMoveLeft(float Value)
{}

void CM2CameraComponent::DoMoveRight(float Value)
{}

void CM2CameraComponent::SetPosition(glm::vec3 Position)
{}

glm::vec3 CM2CameraComponent::GetPosition() const
{
	return glm::vec3();
}

void CM2CameraComponent::SetDirection(glm::vec3 Direction)
{}

glm::vec3 CM2CameraComponent::GetDirection() const
{
	return glm::vec3();
}

void CM2CameraComponent::SetYaw(float Yaw)
{}

void CM2CameraComponent::AddYaw(float Yaw)
{}

float CM2CameraComponent::GetYaw() const
{
	return 0.0f;
}

void CM2CameraComponent::SetPitch(float Pitch)
{}

void CM2CameraComponent::AddPitch(float Pitch)
{}

float CM2CameraComponent::GetPitch() const
{
	return 0.0f;
}

void CM2CameraComponent::SetPerspectiveProjection(float fovy, float aspect, float zNear, float zFar)
{}

void CM2CameraComponent::SetOrthographicProjection(float aspect, float left, float right, float top, float bottom, float zNear, float zFar)
{}

const glm::mat4 & CM2CameraComponent::GetViewMatrix() const
{
	return m_View;
}

const glm::mat4 & CM2CameraComponent::GetInverseViewMatrix() const
{
	return m_Inverse_View;
}

const glm::mat4 & CM2CameraComponent::GetProjectionMatrix() const
{
	return m_Projection;
}

const glm::mat4 & CM2CameraComponent::GetInverseProjectionMatrix() const
{
	return m_Inverse_Projection;
}

const Frustum & CM2CameraComponent::GetFrustum() const
{
	return m_Frustum;
}



//
// IComponentBase
//
void CM2CameraComponent::OnMessage(const ISceneNodeComponent * Component, ComponentMessageType Message)
{
	// Ignore all messages
}

void CM2CameraComponent::Update(const UpdateEventArgs & e)
{
	float aspect = float(GetOwnerNode().GetScene().GetRenderWindow().GetWindowWidth()) / float(GetOwnerNode().GetScene().GetRenderWindow().GetWindowHeight());

	if (false == m_M2Camera->Calculate(GetM2OwnerNode(), e.TotalTime, aspect, &m_Projection, &m_View))
		throw CException("CM2CameraComponent");

	m_Inverse_Projection = glm::inverse(m_Projection);
	m_Inverse_View = glm::inverse(m_View);
	m_Frustum.buildViewFrustum(m_View, m_Projection);
}



//
// Protected
//
const CM2_Base_Instance* CM2CameraComponent::GetM2OwnerNode() const
{
	return dynamic_cast<const CM2_Base_Instance*>(&GetOwnerNode());
}
