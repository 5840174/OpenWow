#pragma once

#include "M2.h"

class ZN_API CM2_Base_Instance 
	: public SceneNode3D
	, public CLoadableObject
{
public:
	CM2_Base_Instance(std::string _m2Name);
	virtual ~CM2_Base_Instance();

	void                                CreateInstances();

	// CM2_Base_Instance
	void                                setM2(std::shared_ptr<M2> _model);
	std::shared_ptr<M2>                 getM2() const { return m_M2; }

	void                                Attach(std::shared_ptr<CM2_Part_Attachment> _attachment);
	void                                Detach();
    std::shared_ptr<CM2_Part_Attachment>GetAttachPoint() const;

	// Color & Alpha
	void                                setColor(vec4 _color) { m_Color = _color; }
	cvec4                               getColor() const { return m_Color; }
	void                                setAlpha(float _alpha) { m_Alpha = _alpha; }
	float                               getAlpha() const { return m_Alpha; }

	// Mesh & textures provider
	virtual bool                        isMeshEnabled(uint32 _index) const;
	void                                setSpecialTexture(SM2_Texture::Type _type, const std::string& _textureName);
	void                                setSpecialTexture(SM2_Texture::Type _type, std::shared_ptr<ITexture> _texture);
	std::shared_ptr<ITexture>           getSpecialTexture(SM2_Texture::Type _type) const;

	// Animations
	std::shared_ptr<CM2_Animator>       getAnimator() const { return m_Animator; }

    // Components
    virtual void                        RegisterComponents() override;

	// ILoadableObject
	virtual bool                        Load() override;

	// SceneNode3D
	void								Initialize() override;
	virtual void                        Accept(IVisitor* visitor) override;

protected:
	virtual void						UpdateLocalTransform();

protected:
	void InitAnimator();

private:
	// Color & Alpha
	vec4                                m_Color;
	float                               m_Alpha;

	// Mesh & Replaces textures
	std::shared_ptr<ITexture>		    m_SpecialTextures[SM2_Texture::Type::COUNT];

	// Animtion
	std::shared_ptr<CM2_Animator>       m_Animator;
	bool                                m_NeedRecalcAnimation;

private:
	std::shared_ptr<M2>                 m_M2;
	std::string                         m_M2Name;
	std::shared_ptr<CM2_Part_Attachment>m_Attached;
};