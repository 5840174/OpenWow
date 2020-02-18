#include "stdafx.h"

// Include
#include "M2_Base_Instance.h"

// General
#include "M2_Part_Texture.h"

CM2_Part_Texture::CM2_Part_Texture(IBaseManager& BaseManager, IRenderDevice& RenderDevice, std::shared_ptr<IFile> f, const SM2_Texture& _proto)
	: m_Texture(nullptr)
{
	m_WrapX = _proto.flags.WRAPX == 0;
	m_WrapY = _proto.flags.WRAPY == 0;

	m_SpecialType = _proto.type;

	// Common texture
	if (m_SpecialType == SM2_Texture::Type::NONE)
	{
		std::string textureFileName = (const char*)(f->getData() + _proto.filename.offset);
		m_Texture = RenderDevice.GetObjectsFactory().LoadTexture2D(textureFileName);
	}
}

CM2_Part_Texture::~CM2_Part_Texture()
{
}

std::shared_ptr<ITexture> CM2_Part_Texture::GetResultTexture(const CM2_Base_Instance* _instance) const
{
	if (isTextureSpecial())
	{
		return _instance->getSpecialTexture(m_SpecialType);
	}

	return getTexture();
}


/*void CM2_Part_Texture::set(RenderState* _state, uint32 _slot, CM2_Base_Instance* _instance) const
{
	uint16 sampler = m_QualitySettings.Texture_Sampler;

	if (m_WrapX)
	{
		sampler |= SS_ADDRU_WRAP;
	}

	if (m_WrapY)
	{
		sampler |= SS_ADDRV_WRAP;
	}

	if (isTextureSpecial())
	{
		std::shared_ptr<ITexture> texture = _instance->getSpecialTexture(m_SpecialType);
		if (texture != nullptr)
		{
			_state->setTexture(_slot, texture, sampler, 0);
		}

		return;
	}

	_ASSERT(getTexture());
	_state->setTexture(_slot, getTexture(), sampler, 0);
}*/
