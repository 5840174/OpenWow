#pragma once

#include "M2.h"

// Components
#include "M2_Animator.h"
#include "M2_Skeleton.h"

class ZN_API CM2_Base_Instance 
	: public SceneNode3D
	, public CLoadableObject
{
public:
	CM2_Base_Instance(const std::shared_ptr<CM2>& M2Object);
	virtual ~CM2_Base_Instance();

	void                                CreateInstances();

	// CLoadableObject
	bool                                Load() override;

	// CM2_Base_Instance
	const CM2&                          getM2() const;

	void                                Attach(std::shared_ptr<const CM2_Part_Attachment> _attachment);
	void                                Detach();
    std::shared_ptr<const CM2_Part_Attachment> GetAttachPoint() const;

	// Color & Alpha
	void                                setColor(vec4 _color) { m_Color = _color; }
	cvec4                               getColor() const { return m_Color; }
	void                                setAlpha(float _alpha) { m_Alpha = _alpha; }
	float                               getAlpha() const { return m_Alpha; }

	// Mesh & textures provider
	virtual bool                        isMeshEnabled(uint32 _index) const;
	void                                setSpecialTexture(SM2_Texture::Type _type, const std::shared_ptr<ITexture>& _texture);
	const std::shared_ptr<ITexture>&    getSpecialTexture(SM2_Texture::Type _type) const;

	// Animations
	const std::shared_ptr<CM2_Animator>&           getAnimator() const { return m_Animator; }
	const std::shared_ptr<CM2SkeletonComponent3D>  getSkeletonComponent() const { return m_SkeletonComponent; }

    // Components
    virtual void                        RegisterComponents() override;

	// SceneNode3D
	void								Initialize() override;
	virtual void                        Update(const UpdateEventArgs& e) override;
	virtual void                        Accept(IVisitor* visitor) override;

protected:
	virtual void						UpdateLocalTransform();

private:
	// Color & Alpha
	vec4                                m_Color;
	float                               m_Alpha;

	// Mesh & Replaces textures
	std::shared_ptr<ITexture>		    m_SpecialTextures[SM2_Texture::Type::COUNT];

	// Animtion
	std::shared_ptr<CM2_Animator>       m_Animator;
	std::shared_ptr<CM2SkeletonComponent3D> m_SkeletonComponent;

private:
	std::shared_ptr<const CM2>           m_M2;
	std::shared_ptr<const CM2_Part_Attachment>m_Attached;
};