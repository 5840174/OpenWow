#pragma once

#ifdef USE_M2_PARTICLES

#include "M2_ParticleSystem.h"

// FORWARD BEGIN
class CM2;
class CM2_Base_Instance;
// FORWARD END


//
// CM2ParticleSystem
//
class CM2ParticleSystem
	: virtual public IParticleSystem
	, public Object
{
public:
	CM2ParticleSystem(const CM2_Base_Instance& M2Instance, IRenderDevice& RenderDevice, const std::shared_ptr<SM2_ParticleSystem_Wrapper>& M2ParticleSystem);
	virtual ~CM2ParticleSystem();

	// CM2ParticleSystem
	std::shared_ptr<IBlendState> GetBlendState() const;

	// IParticleSystem
	void Update(const UpdateEventArgs& e) override;
	void SetNode(const ISceneNode* Node) override;
	const ISceneNode* GetNode() const override;
	void SetEnableCreatingNewParticles(bool Value) override;
	bool IsEnableCreatingNewParticles() const override;
	void SetTexture(const std::shared_ptr<ITexture>& Texture) override;
	std::shared_ptr<ITexture> GetTexture() const override;
	const std::vector<SGPUParticle>& GetGPUParticles() const override;

private:
	std::vector<SGPUParticle> m_ParticleObjects;
	std::shared_ptr<ITexture> m_Texture;
	std::shared_ptr<IBlendState> m_BlendState;

private:
	float rem;
	std::shared_ptr<SM2_ParticleSystem_Wrapper> m_M2ParticleSystem;
	CM2_ParticleObject m_M2ParticleObjects[MAX_PARTICLES];

	const CM2_Base_Instance& m_M2Instance;
};



//
// CM2ParticlesComponent
//
class CM2ParticlesComponent
	: public CParticlesComponent
{
public:
	ZN_OBJECTCLASS(cSceneNodeParticleComponent);

	CM2ParticlesComponent(const CM2_Base_Instance& SceneNode);
	virtual ~CM2ParticlesComponent();

	// IParticleComponent3D
	void AddParticleSystem(std::shared_ptr<IParticleSystem> ParticleSystem) override;
	void RemoveParticleSystem(std::shared_ptr<IParticleSystem> ParticleSystem) override;

protected:
	const CM2_Base_Instance& GetM2OwnerNode() const;
};

#endif