#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Base_Instance.h"

// General
#include "M2_ParticlesComponent.h"

//
// CM2ParticleSystem
//
CM2ParticleSystem::CM2ParticleSystem(IRenderDevice& RenderDevice, const std::shared_ptr<SM2_ParticleSystem_Wrapper>& M2ParticleSystem)
	: m_M2ParticleSystem(M2ParticleSystem)
	, rem(0.0f)
{
	if (m_M2ParticleSystem->GetTexture())
	{
		m_Material = std::make_shared<MaterialParticle>(RenderDevice);
		m_Material->SetTexture(0, m_M2ParticleSystem->GetTexture());
	}

	m_BlendState = RenderDevice.GetObjectsFactory().CreateBlendState();
	m_BlendState->SetBlendMode(m_M2ParticleSystem->GetBlendMode());
}

CM2ParticleSystem::~CM2ParticleSystem()
{
}

//
// CM2ParticleSystem
//
void CM2ParticleSystem::Update(const CM2_Base_Instance * M2Instance, const UpdateEventArgs & e)
{
	m_M2ParticleSystem->update(M2Instance, e, rem, m_M2ParticleObjects);

	m_ParticleObjects.resize(m_M2ParticleObjects.size());
	for (size_t i = 0; i < m_M2ParticleObjects.size(); i++)
	{
		const auto& m2P = m_M2ParticleObjects[i];
		SParticle particle;
		particle.Position = m2P.pos;
		_ASSERT(m2P.tile < m_M2ParticleSystem->GetTiles().size());
		particle.TexCoordBegin = m_M2ParticleSystem->GetTiles()[m2P.tile].tc[1];
		particle.TexCoordEnd = m_M2ParticleSystem->GetTiles()[m2P.tile].tc[3];
		particle.Color = m2P.color;
		particle.Size = glm::vec2(m2P.size) * 1.0f;
		m_ParticleObjects[i] = particle;
	}
}

//
// IParticleSystem
//
void CM2ParticleSystem::AddParticle(const SParticle & Particle)
{
	_ASSERT(false);
}

void CM2ParticleSystem::ClearParticles()
{
	_ASSERT(false);
}

const std::vector<SParticle>& CM2ParticleSystem::GetParticles() const
{
	return m_ParticleObjects;
}

void CM2ParticleSystem::SetMaterial(const std::shared_ptr<IMaterial>& Material)
{
	_ASSERT(false);
}

std::shared_ptr<IMaterial> CM2ParticleSystem::GetMaterial() const
{
	return m_Material;
}

std::shared_ptr<IBlendState> CM2ParticleSystem::GetBlendState() const
{
	return m_BlendState;
}

//
// CM2ParticlesComponent3D
//
CM2ParticlesComponent3D::CM2ParticlesComponent3D(const CM2_Base_Instance& SceneNode)
	: CComponentBase(SceneNode)
{

	//for (size_t i = 0; i < 1; i++)
	//{
	//	const auto& p = GetM2OwnerNode().getM2().getMiscellaneous().GetParticles().at(i);
	//	m_ParticleSystems.push_back(std::make_shared<CM2ParticleSystem>(GetM2OwnerNode().getM2().GetRenderDevice(), p));
	//}

	for (const auto& m2ParticleSystem : GetM2OwnerNode().getM2().getMiscellaneous().GetParticles())
	{
		m_ParticleSystems.push_back(std::make_shared<CM2ParticleSystem>(GetM2OwnerNode().getM2().GetRenderDevice(), m2ParticleSystem));
	}
}

CM2ParticlesComponent3D::~CM2ParticlesComponent3D()
{
}

//
// ISceneNodeComponent
//
void CM2ParticlesComponent3D::Update(const UpdateEventArgs& e)
{
	for (auto& it : m_ParticleSystems)
	{
		it->Update(&GetM2OwnerNode(), e);
	}
}

void CM2ParticlesComponent3D::Accept(IVisitor * visitor)
{
	for (auto& it : m_ParticleSystems)
	{
		visitor->Visit((const IParticleSystem*)it.get());
	}
}

//
// Protected
//
const CM2_Base_Instance& CM2ParticlesComponent3D::GetM2OwnerNode() const
{
	return reinterpret_cast<const CM2_Base_Instance&>(GetOwnerNode());
}
