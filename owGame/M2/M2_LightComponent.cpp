#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Base_Instance.h"

// General
#include "M2_LightComponent.h"

CM2LightComponent::CM2LightComponent(const CM2_Base_Instance& OwnerNode)
	: CSceneNodeComponentBase(OwnerNode)
{
	for (const auto& m2Light : OwnerNode.GetM2().getMiscellaneous().GetLightsDirectArray())
		m_Lights.push_back(MakeShared(CM2Light, OwnerNode.GetBaseManager(), *m2Light));
}

CM2LightComponent::~CM2LightComponent()
{}



//
// ILightComponent3D
//
void CM2LightComponent::SetLight(std::shared_ptr<ILight> Light)
{
	throw CException("CM2LightComponent::SetLight unexpected behaviour.");
}

std::shared_ptr<ILight> CM2LightComponent::GetLight() const
{
	throw CException("CM2LightComponent::GetLight unexpected behaviour.");
}



//
// ISceneNodeComponent
//
void CM2LightComponent::OnMessage(const ISceneNodeComponent * Component, ComponentMessageType Message)
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

void CM2LightComponent::Update(const UpdateEventArgs & e)
{
	for (const auto& light : m_Lights)
	{
		light->Calculate(GetM2OwnerNode(), e.TotalTime);
	}
}

void CM2LightComponent::Accept(IVisitor * visitor)
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

const CM2_Base_Instance & CM2LightComponent::GetM2OwnerNode() const
{
	return dynamic_cast<const CM2_Base_Instance&>(GetOwnerNode());
}
