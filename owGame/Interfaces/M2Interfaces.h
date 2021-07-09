#pragma once

// FORWARD BEGIN
class CM2;
class CM2_Animator;
class CM2SkeletonComponent;
class CM2ParticlesComponent;
// FORWARD END

ZN_INTERFACE ZN_API IowM2BaseInstance
{
	virtual ~IowM2BaseInstance() {}

	virtual void CreateInstances() = 0;

	// CM2_Base_Instance
	virtual const CM2& getM2() const = 0;

	virtual void Attach(EM2_AttachmentPoint AttachmentType) = 0;
	virtual void Detach() = 0;
	virtual void UpdateAttachPositionAfterSkeletonUpdate() = 0;

	// Color & Alpha
	virtual void setColor(glm::vec4 _color) = 0;
	virtual const glm::vec4& getColor() const = 0;
	virtual void setAlpha(float _alpha) = 0;
	virtual float getAlpha() const = 0;

	// Mesh & textures provider
	virtual bool isMeshEnabled(uint32 _index) const = 0;
	virtual void setSpecialTexture(SM2_Texture::Type _type, const std::shared_ptr<ITexture>& _texture) = 0;
	virtual const std::shared_ptr<ITexture>& getSpecialTexture(SM2_Texture::Type _type) const = 0;

	// Animations
	virtual const std::shared_ptr<CM2_Animator>& getAnimator() const = 0;
	virtual const std::shared_ptr<CM2SkeletonComponent> getSkeletonComponent() const = 0;
	virtual const std::shared_ptr<CM2ParticlesComponent> getParticleComponent() const = 0;
};