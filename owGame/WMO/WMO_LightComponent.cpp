#include "stdafx.h"

// Include
#include "WMO.h"
#include "WMO_Base_Instance.h"

// General
#include "WMO_LightComponent.h"

CWMOLightComponent::CWMOLightComponent(const CWMO_Base_Instance& OwnerNode)
	: CSceneNodeComponentBase(OwnerNode)
{
	for (const auto& wmoLight : OwnerNode.GetWMO().GetLights())
		m_Lights.push_back(MakeShared(CWMOLight, OwnerNode.GetBaseManager(), *wmoLight));
}

CWMOLightComponent::~CWMOLightComponent()
{}



//
// ILightComponent3D
//
void CWMOLightComponent::SetLight(std::shared_ptr<ILight> Light)
{
	throw CException("CWMOLightComponent::SetLight unexpected behaviour.");
}

std::shared_ptr<ILight> CWMOLightComponent::GetLight() const
{
	throw CException("CWMOLightComponent::GetLight unexpected behaviour.");
}



//
// ISceneNodeComponent
//
void CWMOLightComponent::OnMessage(const ISceneNodeComponent * Component, ComponentMessageType Message)
{
	if (Message == UUID_OnWorldTransformChanged)
	{
		for (const auto& light : m_Lights)
		{
			glm::mat4 matrix = GetOwnerNode().GetWorldTransfom();

			glm::vec3 lightPosition = matrix * glm::vec4(light->GetLightPosition(), 1.0f);
			glm::vec3 lightDirection = matrix * glm::vec4(lightPosition, 1.0f);

			std::dynamic_pointer_cast<ILightInternal>(light)->SetPosition(lightPosition);
			//std::dynamic_pointer_cast<ILightInternal>(light)->SetRotation(GetOwnerNode().GetLocalRotationDirection());
		}
	}
}

void CWMOLightComponent::Update(const UpdateEventArgs & e)
{
	for (const auto& light : m_Lights)
	{
		light->Calculate(GetWMOOwnerNode(), e.TotalTime);
	}
}

void CWMOLightComponent::Accept(IVisitor * visitor)
{
	for (const auto& light : m_Lights)
	{
		if (false == light->IsEnabled())
			continue;

		if (light->GetType() == ELightType::Directional)
			continue;

		visitor->Visit(light);
	}
}



//
// Protected
//
const CWMO_Base_Instance & CWMOLightComponent::GetWMOOwnerNode() const
{
	return dynamic_cast<const CWMO_Base_Instance&>(GetOwnerNode());
}
