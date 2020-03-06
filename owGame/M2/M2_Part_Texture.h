#pragma once

#include "M2_Types.h"

// FORWARD BEGIN
class CM2_Base_Instance;
// FORWARD END

class CM2_Part_Texture
{
public:
	CM2_Part_Texture(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const CM2& M2Object, const std::shared_ptr<IFile>& File, const SM2_Texture& M2Texture);
	virtual ~CM2_Part_Texture();

	ISamplerState::WrapMode GetTextureWrapX() const { return (m_WrapX ? ISamplerState::WrapMode::Clamp : ISamplerState::WrapMode::Repeat); }
	ISamplerState::WrapMode GetTextureWrapY() const { return (m_WrapY ? ISamplerState::WrapMode::Clamp : ISamplerState::WrapMode::Repeat); }

	std::shared_ptr<ITexture> GetTexture() const;
	std::shared_ptr<ITexture> GetTexture(const CM2_Base_Instance* _instance) const;
	bool isTextureSpecial() const { return (m_SpecialType != SM2_Texture::Type::NONE); }
	SM2_Texture::Type getSpecialTextureType() const { return m_SpecialType; }

private:
	bool                       m_WrapX;
	bool                       m_WrapY;

	std::shared_ptr<ITexture>  m_Texture;
	SM2_Texture::Type          m_SpecialType;

private:
	const CM2& m_M2Object;
};