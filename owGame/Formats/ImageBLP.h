#pragma once

namespace BLPFormat
{
	#define LIBBLP_MIPMAP_COUNT 16
	#define LIBBLP_PALETTE_SIZE 256

	enum BLPColorEncoding : uint8_t {
		COLOR_JPEG = 0, // not supported
		COLOR_PALETTE = 1,
		COLOR_DXT = 2,
		COLOR_ARGB8888 = 3,
		COLOR_ARGB8888_dup = 4,    // same decompression, likely other PIXEL_FORMAT
	};

	enum BLPPixelFormat : uint8_t {
		PIXEL_DXT1 = 0,
		PIXEL_DXT3 = 1,
		PIXEL_ARGB8888 = 2,
		PIXEL_ARGB1555 = 3,
		PIXEL_ARGB4444 = 4,
		PIXEL_RGB565 = 5,
		PIXEL_DXT5 = 7,
		PIXEL_UNSPECIFIED = 8,
		PIXEL_ARGB2565 = 9,
		NUM_PIXEL_FORMATS = 10,
	};

	/* define errors. */
	#define LIBBLP_SUCCESS				     0		/* success. */
	#define LIBBLP_ERROR_OPEN				-1		/* open error on file. */
	#define LIBBLP_ERROR_CLOSE				-2		/* close error on file. */
	#define LIBBLP_ERROR_SEEK				-3		/* lseek error on file. */
	#define LIBBLP_ERROR_MALLOC				-6		/* memory allocation error. */
	#define LIBBLP_ERROR_FORMAT				-7		/* format errror. */
	#define LIBBLP_ERROR_SIZE				-9		/* buffer size is to small. */

	struct BLPHeader
	{
		uint8_t    magic[4];
		uint32_t     type;

		BLPColorEncoding colorEncoding;
		uint8_t    alphaChannelBitDepth;  // Alpha channel bit depth: 0, 1, 4 or 8
		BLPPixelFormat   pixelFormat;   // 0, 1, 7, or 8
		uint8_t    has_mips;     // 0 = no mips, 1 = has mips

		uint32_t     width;
		uint32_t     height;
		uint32_t     mipOffsets[LIBBLP_MIPMAP_COUNT];
		uint32_t     mipSizes[LIBBLP_MIPMAP_COUNT];
		uint32_t     pallete[LIBBLP_PALETTE_SIZE];
	};

}

class CImageBLP
	: public CImageBase
{
public:
	CImageBLP();
	virtual ~CImageBLP();

protected:
	bool LoadImageData(std::shared_ptr<IFile> File);
	bool LoadBPL(const BLPFormat::BLPHeader& header, std::shared_ptr<IFile> f);

	template <class DECODER>
	bool LoadDXT_Helper(std::shared_ptr<IFile> io);

public: // Static
	static bool IsFileSupported(std::shared_ptr<IFile> File);
	static std::shared_ptr<CImageBLP> CreateImage(std::shared_ptr<IFile> File);

};