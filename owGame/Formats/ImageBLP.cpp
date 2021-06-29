#include "stdafx.h"

#include "ImageBLP.h"


CImageBLP::CImageBLP(const std::string & FileName)
	: CImageBase(FileName)
{
}

CImageBLP::CImageBLP(const std::string & FileName, uint32 Width, uint32 Height, uint32 BitsPerPixel)
	: CImageBase(FileName, Width, Height, BitsPerPixel, false)
{}

CImageBLP::~CImageBLP()
{
}

bool CImageBLP::LoadImageData(std::shared_ptr<IFile> File)
{
	BLPFormat::BLPHeader header = { 0 };
	File->seek(0);
	File->read(&header);
	return LoadBPL(header, File);
}

bool CImageBLP::IsFilenameSupported(const std::string & Filename)
{
	auto lastPointPos = Filename.find_last_of('.');
	if (lastPointPos != std::string::npos)
	{
		std::string extension = Filename.substr(lastPointPos + 1);
		extension = Utils::ToLower(extension);
		return extension == "blp";
	}

	return false;
}

bool CImageBLP::IsFileSupported(std::shared_ptr<IFile> File)
{
	_ASSERT(File != nullptr);

	if (false == File->Extension().empty())
		if (Utils::ToLower(File->Extension()) != "blp")
			return false;

	if (File->getSize() == 0)
		return false;

	BLPFormat::BLPHeader header = { 0 };
	File->seek(0);
	File->read(&header);

	return (header.magic[0] == 'B' && header.magic[1] == 'L' && header.magic[2] == 'P' && header.magic[3] == '2' && header.type == 1);
}

std::shared_ptr<CImageBLP> CImageBLP::CreateEmptyImage(const std::string & FileName, uint32 Width, uint32 Height, uint32 BitsPerPixel)
{
	return MakeShared(CImageBLP, FileName, Width, Height, BitsPerPixel);;
}

std::shared_ptr<CImageBLP> CImageBLP::CreateImage(std::shared_ptr<IFile> File)
{
	_ASSERT(IsFileSupported(File));

	std::shared_ptr<CImageBLP> imageBLP = MakeShared(CImageBLP, File->Path_Name());
	if (false == imageBLP->LoadImageData(File))
	{
		Log::Error("CImageBLP: Unable to load PLP file '%s'.", File->Name().c_str());
		return nullptr;
	}

	return imageBLP;
}

bool CImageBLP::LoadBPL(const BLPFormat::BLPHeader& header, std::shared_ptr<IFile> f)
{
	if (header.width & (header.width - 1))
	{
		//view->IsTexture3D = true;
		// 3D texture
		return false;
	}

	if (header.height & (header.height - 1))
	{
		_ASSERT(false);
		return false;
	}

	m_Width = header.width;
	m_Height = header.height;
	m_BitsPerPixel = 32;
	m_IsTransperent = (header.alphaChannelBitDepth != 0);

	bool    hasAlpha = (header.alphaChannelBitDepth != 0);
	uint8_t mipmax = (header.has_mips ? LIBBLP_MIPMAP_COUNT : 1);

	m_MipsCount = mipmax;

	for (uint8_t currentMip = 0; currentMip < mipmax; currentMip++)
	{
		if ((header.mipOffsets[currentMip] == 0) || (header.mipSizes[currentMip] == 0))
		{
			m_MipsCount = currentMip;
			break;
		}

		uint16 mipWidth = std::max(header.width >> currentMip, 1u);
		uint16 mipHeight = std::max(header.height >> currentMip, 1u);
		uint32 mipStride = mipWidth * (m_BitsPerPixel / 8);

		m_MipMapData[currentMip].resize(mipHeight * mipStride);

		switch (header.colorEncoding)
		{
		case BLPFormat::BLPColorEncoding::COLOR_PALETTE:
		{
			// Data in mipmaps in indices info pallete
			uint8_t* indexInPalleteBuffer = new uint8_t[header.mipSizes[currentMip]];
			f->seek(header.mipOffsets[currentMip]);
			f->readBytes(indexInPalleteBuffer, header.mipSizes[currentMip]);

			//view->MipData[currentMip] = new uint8_t[header.width * header.height * 4];
			uint32_t resultBufferCntr = 0;

			uint8_t* indexInPalleteColor = indexInPalleteBuffer;
			int alphaBitCntr = 0;
			uint8_t* indexInPalleteAlpha = &indexInPalleteBuffer[0] + mipWidth * mipHeight;

			for (uint32_t y = 0; y < mipWidth; y++)
			{
				for (uint32_t x = 0; x < mipHeight; x++)
				{
					// Read color
					uint32_t color = header.pallete[*indexInPalleteColor++];
					//color = ((color & 0x00FF0000) >> 16) | ((color & 0x0000FF00)) | ((color & 0x000000FF) << 16);

					// Read alpha
					uint8_t alpha;
					switch (header.alphaChannelBitDepth)
					{
					case 0:
						alpha = 0xff;
						break;
					case 1:
						alpha = (*indexInPalleteAlpha & (1 << alphaBitCntr++)) ? 0xff : 0x00;
						if (alphaBitCntr == 8)
						{
							alphaBitCntr = 0;
							indexInPalleteAlpha++;
						}
						break;
					case 4:
						alpha = 0xFF;
						break;
					case 8:
						alpha = (*indexInPalleteAlpha++);
						break;
					default:
						_ASSERT(false); //LIBBLP_ERROR_FORMAT
						return NULL;
					}

					m_MipMapData[currentMip][resultBufferCntr++] = ((color & 0x00FF0000) >> 16);
					m_MipMapData[currentMip][resultBufferCntr++] = ((color & 0x0000FF00) >> 8);
					m_MipMapData[currentMip][resultBufferCntr++] = ((color & 0x000000FF));
					m_MipMapData[currentMip][resultBufferCntr++] = ((alpha & 0x000000FF));
				}
			}

			delete[] indexInPalleteBuffer;
		}
		break;

		case BLPFormat::BLPColorEncoding::COLOR_DXT:
		{
			f->seek(header.mipOffsets[currentMip]);

			switch (header.pixelFormat)
			{
			case BLPFormat::BLPPixelFormat::PIXEL_DXT1:
				LoadDXT_Helper <DDSFormat::DXT_BLOCKDECODER_1>(mipWidth, mipHeight, mipStride, GetDataEx(currentMip), f);
				break;
			case BLPFormat::BLPPixelFormat::PIXEL_DXT3:
				LoadDXT_Helper <DDSFormat::DXT_BLOCKDECODER_3>(mipWidth, mipHeight, mipStride, GetDataEx(currentMip), f);
				break;
			case BLPFormat::BLPPixelFormat::PIXEL_DXT5:
				LoadDXT_Helper <DDSFormat::DXT_BLOCKDECODER_5>(mipWidth, mipHeight, mipStride, GetDataEx(currentMip), f);
				break;
			default:
				_ASSERT(false); //LIBBLP_ERROR_FORMAT
				return false;
			}
		}
		break;

		case BLPFormat::BLPColorEncoding::COLOR_ARGB8888:
		{
			f->seek(header.mipOffsets[currentMip]);
			f->readBytes(GetDataEx(currentMip), header.mipSizes[currentMip]);
		}
		break;

		default:
			_ASSERT(false); //LIBBLP_ERROR_FORMAT
			return false;
		}
	}

	return true;
}
