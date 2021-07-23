#include "stdafx.h"

// Include
#include "M2_Base_Instance.h"

// General
#include "M2_Part_Texture.h"

CM2_Part_Texture::CM2_Part_Texture(const IBaseManager& BaseManager, IRenderDevice& RenderDevice, const CM2& M2Object, const std::shared_ptr<IByteBuffer>& File, const SM2_Texture& M2Texture)
	: m_M2Object(M2Object)
{
	m_WrapX = (M2Texture.flags.WRAPX == 0);
	m_WrapY = (M2Texture.flags.WRAPY == 0);

	m_SpecialType = M2Texture.type;

	// Common texture
	if (m_SpecialType == SM2_Texture::Type::NONE)
	{
		std::string textureFileName = (const char*)(File->getData() + M2Texture.filename.offset);
		m_Texture = BaseManager.GetManager<IznTexturesFactory>()->LoadTexture2D(textureFileName);
	}
}

CM2_Part_Texture::~CM2_Part_Texture()
{
}

const std::shared_ptr<ITexture>& CM2_Part_Texture::GetTexture() const
{
	if (m_SpecialType != SM2_Texture::Type::NONE)
		throw CException("CM2_Part_Texture::GetTexture: You can access only NONE texture using getter without instance.");
	return m_Texture;
}

const std::shared_ptr<ITexture>& CM2_Part_Texture::GetTexture(const CM2_Base_Instance* M2Instance) const
{
	if (m_SpecialType != SM2_Texture::Type::NONE)
		return M2Instance->GetSpecialTexture(m_SpecialType);
	return m_Texture;
}
