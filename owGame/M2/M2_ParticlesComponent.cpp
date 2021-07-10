#include "stdafx.h"

#ifdef USE_M2_PARTICLES

// Include
#include "M2.h"
#include "M2_Base_Instance.h"

// General
#include "M2_ParticlesComponent.h"

//
// CM2ParticleSystem
//
CM2ParticleSystem::CM2ParticleSystem(const CM2_Base_Instance& M2Instance, IRenderDevice& RenderDevice, const std::shared_ptr<CM2_Part_ParticleSystem>& M2ParticleSystem)
	: Object(RenderDevice.GetBaseManager())
	, m_M2Instance(M2Instance)
	, m_M2ParticleSystem(M2ParticleSystem)
	, rem(0.0f)
{
	m_Texture = m_M2ParticleSystem->GetTexture();

	m_BlendState = RenderDevice.GetObjectsFactory().CreateBlendState();
	m_BlendState->SetBlendMode(m_M2ParticleSystem->GetBlendMode());
}

CM2ParticleSystem::~CM2ParticleSystem()
{
}



//
// CM2ParticleSystem
//
std::shared_ptr<IBlendState> CM2ParticleSystem::GetBlendState() const
{
	return m_BlendState;
}



//
// IParticleSystem
//
void CM2ParticleSystem::Update(const UpdateEventArgs & e)
{
	m_M2ParticleSystem->update(&m_M2Instance, e, &rem, m_M2ParticleObjects);

	m_ParticleObjects.clear();

	for (size_t i = 0; i < 100; i++)
	{
		const auto& m2P = m_M2ParticleObjects[i];
		if (false == m2P.Active)
			continue;

		SGPUParticle particle = m2P.ToGPUParticle();
		_ASSERT(m2P.tile < m_M2ParticleSystem->GetTiles().size());
		particle.TexCoordBegin = m_M2ParticleSystem->GetTiles()[m2P.tile].Start;
		particle.TexCoordEnd = m_M2ParticleSystem->GetTiles()[m2P.tile].End;
		m_ParticleObjects.push_back(particle);
	}
}

void CM2ParticleSystem::SetNode(const ISceneNode * Node)
{

}

const ISceneNode * CM2ParticleSystem::GetNode() const
{
	return nullptr;
}

void CM2ParticleSystem::SetEnableCreatingNewParticles(bool Value)
{}

bool CM2ParticleSystem::IsEnableCreatingNewParticles() const
{
	return true;
}

void CM2ParticleSystem::SetTexture(const std::shared_ptr<ITexture>& Texture)
{
	_ASSERT(false);
}

std::shared_ptr<ITexture> CM2ParticleSystem::GetTexture() const
{
	return m_Texture;
}

const std::vector<SGPUParticle>& CM2ParticleSystem::GetGPUParticles() const
{
	return m_ParticleObjects;
}



//
// CM2ParticlesComponent
//
CM2ParticlesComponent::CM2ParticlesComponent(const CM2_Base_Instance& SceneNode)
	: CParticlesComponent(SceneNode)
{
	for (const auto& m2ParticleSystem : GetM2OwnerNode().GetM2().getMiscellaneous().GetParticles())
	{
		AddParticleSystem(MakeShared(CM2ParticleSystem, GetM2OwnerNode(), GetM2OwnerNode().GetM2().GetRenderDevice(), m2ParticleSystem));
	}
}

CM2ParticlesComponent::~CM2ParticlesComponent()
{}

//
// IParticleComponent
//
void CM2ParticlesComponent::AddParticleSystem(std::shared_ptr<IParticleSystem> ParticleSystem)
{
	const auto& it = std::find(m_ParticleSystems.begin(), m_ParticleSystems.end(), ParticleSystem);
	if (it != m_ParticleSystems.end())
		throw CException("Particle system already exists.");

	ParticleSystem->SetNode(&GetOwnerNode());
	m_ParticleSystems.push_back(ParticleSystem);

	// Add proxy properties
	//auto particleSystemPropertiesProxy = MakeShared(CPropertyGroupProxy, ParticleSystem->GetProperties());
	//GetProperties()->AddProperty(particleSystemPropertiesProxy);
}

void CM2ParticlesComponent::RemoveParticleSystem(std::shared_ptr<IParticleSystem> ParticleSystem)
{
	auto it = std::find(m_ParticleSystems.begin(), m_ParticleSystems.end(), ParticleSystem);
	if (it == m_ParticleSystems.end())
		throw CException("Particle system don't found.");

	(*it)->SetNode(nullptr);
	m_ParticleSystems.erase(it);

	//GetProperties()->RemoveProperty(ParticleSystem->GetProperties());
}

//
// Protected
//
const CM2_Base_Instance& CM2ParticlesComponent::GetM2OwnerNode() const
{
	return dynamic_cast<const CM2_Base_Instance&>(GetOwnerNode());
}

#endif
