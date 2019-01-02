#pragma once

#include "M2.h"

class CM2_Base_Instance : public SceneNode
{
public:
	CM2_Base_Instance(std::shared_ptr<M2> _m2Object = nullptr);
	virtual ~CM2_Base_Instance();

	// CM2_Base_Instance
	void setM2(std::shared_ptr<M2> _model);
	std::shared_ptr<M2> getM2() const { return m_M2; }

	void Load();

	void Attach(const CM2_Part_Attachment* _attachment);
	void Detach();

	// Color & Alpha
	void setColor(vec4 _color) { m_Color = _color; }
	cvec4 getColor() const { return m_Color; }
	void setAlpha(float _alpha) { m_Alpha = _alpha; }
	float getAlpha() const { return m_Alpha; }

	// Mesh & textures provider
	virtual bool isMeshEnabled(uint32 _index) const;
	void setSpecialTexture(SM2_Texture::Type _type, cstring _textureName);
	void setSpecialTexture(SM2_Texture::Type _type, std::shared_ptr<Texture> _texture);
	std::shared_ptr<Texture> getSpecialTexture(SM2_Texture::Type _type) const;

	// Animations
	std::shared_ptr<CM2_Animator> getAnimator() { return m_Animator; }
	double m_Time;  // TODO: Delete me!!!
	double m_DTime; // TODO: Delete me!!!

	// IUpdatable
	void Update(double _time, double _dTime);

	// SceneNode
	void Accept(IVisitor& visitor) override;

protected:
	void InitLocal();
	virtual void CalculateLocalTransform(bool _isRotationQuat = false) override;

private:
	// Color & Alpha
	vec4                            m_Color;
	float                           m_Alpha;

	// Mesh & Replaces textures
	std::shared_ptr<Texture>		m_SpecialTextures[SM2_Texture::Type::COUNT];

	// Animtion
	std::shared_ptr<CM2_Animator>   m_Animator;
	bool                            m_NeedRecalcAnimation;

private: // PARENT
	std::shared_ptr<M2>             m_M2;
	const CM2_Part_Attachment*      m_Attached;
};