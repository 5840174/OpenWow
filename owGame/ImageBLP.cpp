#include "stdafx.h"

#include "ImageBLP.h"

CImageBLP::CImageBLP()
{
}

CImageBLP::~CImageBLP()
{
}

bool CImageBLP::LoadImageData(std::shared_ptr<IFile> File)
{
	ImageBLP::BLPHeader header = { 0 };
	File->seek(0);
	File->read(&header);
	return LoadBPL(header, File);
}

bool CImageBLP::IsFileSupported(std::shared_ptr<IFile> File)
{
	_ASSERT(File != nullptr);

	if (File->Extension().empty() == false)
		if (Utils::ToLower(File->Extension()) != "blp")
			return false;

	ImageBLP::BLPHeader header = { 0 };
	File->seek(0);
	File->read(&header);
	return (header.magic[0] == 'B' && header.magic[1] == 'L' && header.magic[2] == 'P' && header.magic[3] == '2' && header.type == 1);
}

std::shared_ptr<CImageBLP> CImageBLP::CreateImage(std::shared_ptr<IFile> File)
{
	_ASSERT(IsFileSupported(File));

	std::shared_ptr<CImageBLP> imageBLP = std::make_shared<CImageBLP>();
	if (!imageBLP->LoadImageData(File))
	{
		Log::Error("CImageBLP: Unable to load PLP file '%s'.", File->Name().c_str());
		return nullptr;
	}

	return imageBLP;
}

bool CImageBLP::LoadBPL(const ImageBLP::BLPHeader& header, std::shared_ptr<IFile> f)
{
	if (header.width & (header.width - 1))
	{
		//view->IsTexture3D = true;
		// 3D texture
	}
	if (header.height & (header.height - 1))
	{
		_ASSERT(false);
		return false;
	}

	m_Width = header.width;
	m_Height = header.height;
	m_BitsPerPixel = 32;
	m_Stride = m_Width * (m_BitsPerPixel / 8);
	m_IsTransperent = (header.alphaChannelBitDepth != 0);
	m_Data = new uint8[m_Height * m_Stride];


	bool    hasAlpha = (header.alphaChannelBitDepth != 0);
	uint8_t mipmax = 1; //(header.has_mips ? LIBBLP_MIPMAP_COUNT : 1);

	for (uint8_t currentMip = 0; currentMip < mipmax; currentMip++)
	{
		if ((header.mipOffsets[currentMip] == 0) || (header.mipSizes[currentMip] == 0))
			break;

		uint16_t mipWidth = std::max(header.width >> currentMip, 1u);
		uint16_t mipHeight = std::max(header.height >> currentMip, 1u);

		switch (header.colorEncoding)
		{
		case ImageBLP::BLPColorEncoding::COLOR_PALETTE:
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
					case 8:
						alpha = (*indexInPalleteAlpha++);
						break;
					default:
						_ASSERT(false); //LIBBLP_ERROR_FORMAT
						return NULL;
					}

					m_Data[resultBufferCntr++] = ((color & 0x000000FF));
					m_Data[resultBufferCntr++] = ((color & 0x0000FF00) >> 8);
					m_Data[resultBufferCntr++] = ((color & 0x00FF0000) >> 16);
					m_Data[resultBufferCntr++] = ((alpha & 0x000000FF));
				}
			}

			delete[] indexInPalleteBuffer;
		}
		break;

		case ImageBLP::BLPColorEncoding::COLOR_DXT:
		{
			f->seek(header.mipOffsets[currentMip]);

			switch (header.pixelFormat)
			{
			case ImageBLP::BLPPixelFormat::PIXEL_DXT1:
				LoadDXT_Helper <DXT_BLOCKDECODER_1>(f);
				break;
			case ImageBLP::BLPPixelFormat::PIXEL_DXT3:
				LoadDXT_Helper <DXT_BLOCKDECODER_3>(f);
				break;
			case ImageBLP::BLPPixelFormat::PIXEL_DXT5:
				LoadDXT_Helper <DXT_BLOCKDECODER_5>(f);
				break;
			default:
				_ASSERT(false); //LIBBLP_ERROR_FORMAT
				return false;
			}
		}
		break;

		case ImageBLP::BLPColorEncoding::COLOR_ARGB8888:
		{
			f->seek(header.mipOffsets[currentMip]);
			f->readBytes(m_Data, header.mipSizes[currentMip]);
		}
		break;

		default:
			_ASSERT(false); //LIBBLP_ERROR_FORMAT
			return false;
		}
	}

	return true;
}

template <class DECODER>
bool CImageBLP::LoadDXT_Helper(std::shared_ptr<IFile> io)
{
	typedef typename DECODER::INFO INFO;
	typedef typename INFO::Block Block;

	Block *input_buffer = new Block[(m_Width + 3) / 4];
	if (!input_buffer)
	{
		Log::Error("CImageDDS: LoadDXT_Helper: Unable to create 'Block' array with size '%d'.", (m_Width + 3) / 4);
		return false;
	}

	int widthRest = (int)m_Width & 3;
	int heightRest = (int)m_Height & 3;
	int inputLine = (m_Width + 3) / 4;
	uint32 y = 0;

	if (m_Height >= 4)
	{
		for (; y < m_Height; y += 4)
		{
			io->readBytes(input_buffer, sizeof(typename INFO::Block) * inputLine);

			// TODO: probably need some endian work here
			BYTE *pbSrc = (BYTE *)input_buffer;
			BYTE *pbDst = GetLine(y);

			if (m_Width >= 4)
			{
				for (uint32 x = 0; x < m_Width; x += 4)
				{
					DecodeDXTBlock <DECODER>(pbDst, pbSrc, m_Stride, 4, 4);
					pbSrc += INFO::bytesPerBlock;
					pbDst += 4 * 4;
				}
			}
			if (widthRest)
			{
				DecodeDXTBlock <DECODER>(pbDst, pbSrc, m_Stride, widthRest, 4);
			}
		}
	}

	if (heightRest)
	{
		io->readBytes(input_buffer, sizeof(typename INFO::Block) * inputLine);
		// TODO: probably need some endian work here
		BYTE *pbSrc = (BYTE *)input_buffer;
		BYTE *pbDst = GetLine(y);

		if (m_Width >= 4)
		{
			for (int x = 0; x < m_Width; x += 4)
			{
				DecodeDXTBlock <DECODER>(pbDst, pbSrc, m_Stride, 4, heightRest);
				pbSrc += INFO::bytesPerBlock;
				pbDst += 4 * 4;
			}
		}
		if (widthRest)
		{
			DecodeDXTBlock <DECODER>(pbDst, pbSrc, m_Stride, widthRest, heightRest);
		}

	}

	delete[] input_buffer;

	for (size_t pixelIndex = 0; pixelIndex < m_Height * m_Width * 4; pixelIndex += 4)
	{
		if (m_Data[pixelIndex + 3] < 0xFF)
		{
			m_IsTransperent = true;
			break;
		}
	}

	return true;
}