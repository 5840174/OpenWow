#include "stdafx.h"

// Include
#include "MapTile.h"
#include "Map.h"

// General
#include "MapChunk.h"

// Additional
#include "MapChunkLiquid.h"
#include "Map_Shared.h"
#include "MapChunkMaterial.h"

namespace
{
	inline uint8 lerpUInt8(uint8 a, uint8 b, uint8 f)
	{
		const float aF = static_cast<float>(a) / 255.0f;
		const float bF = static_cast<float>(b) / 255.0f;
		const float fF = static_cast<float>(f) / 255.0f;
		const float resultF = (aF  * (1.0f - fF)) + (bF * fF);
		return static_cast<uint8>(glm::round(resultF * 255.0f));
	}
}

CMapChunk::CMapChunk(IRenderDevice& RenderDevice, const CMap& Map, const std::shared_ptr<CMapTile>& MapTile, const ADT_MCIN& Chunk, const std::shared_ptr<IByteBuffer>& Bytes)
	: CLoadableObject(MapTile)
	, m_RenderDevice(RenderDevice)
	, m_Map(Map)
	, m_MapTile(*MapTile)
	, m_Bytes(Bytes)
{
	m_Bytes = std::make_shared<CByteBuffer>(Bytes->getData() + Chunk.offset, Chunk.size);
	SetType(cMapChunk_NodeType);
	SetName("Chunk [" + std::to_string(Chunk.offset) + "]");
}

CMapChunk::~CMapChunk()
{}


uint32 CMapChunk::GetAreaID() const
{
    return m_AreaID;
}


//
// ISceneNode
//
void CMapChunk::Initialize()
{
	//m_Bytes->seek(m_MCIN.offset);

// Chunk + size (8)
	uint32_t offset;
	m_Bytes->read(&offset);

	uint32_t size;
	m_Bytes->read(&size);

	SetName("MapTile[" + std::to_string(m_MapTile.getIndexX()) + "," + std::to_string(m_MapTile.getIndexZ()) + "]_Chunk[" + std::to_string(offset) + "]");

	uint32_t startPos = m_Bytes->getPos();

	// Read header
	m_Bytes->readBytes(&header, sizeof(ADT_MCNK_Header));

	m_AreaID = header.areaid;

	// Scene node params
	{
		// Set translate
		SetTranslate(glm::vec3(header.xpos * (-1.0f) + C_ZeroPoint, header.ypos, header.zpos * (-1.0f) + C_ZeroPoint));
	}

	{
		glm::vec3 translate = GetTranslation();

		// Bounds
		BoundingBox bbox
		(
			glm::vec3(0.0f, Math::MaxFloat, 0.0f),
			glm::vec3(0.0f + C_ChunkSize, Math::MinFloat, 0.0f + C_ChunkSize)
		);

		GetColliderComponent()->SetCullDistance(m_RenderDevice.GetBaseManager().GetManager<ISettings>()->GetGroup("WoWSettings")->GetSettingT<float>("ADT_MCNK_Distance")->Get());
		GetColliderComponent()->SetBounds(bbox);
		GetColliderComponent()->SetDebugDrawMode(false);
	}
}

//
// ILoadable
//
bool CMapChunk::Load()
{
	if (auto depend = GetDependense().lock())
		if (depend->GetState() == ILoadable::ELoadableState::Deleted)
			return false;

	uint32_t startPos = m_Bytes->getPos() - sizeof(ADT_MCNK_Header);

	std::shared_ptr<IBuffer> verticesBuffer = nullptr;
	std::shared_ptr<IBuffer> normalsBuffer = nullptr;

	// Normals
	m_Bytes->seek(startPos + header.ofsNormal);
	{
		struct int24
		{
			int24() :
				x(0),
				y(0),
				z(0)
			{}

			int24(int8 _x, int8 _y, int8 _z) :
				x(_x),
				y(_y),
				z(_z)
			{}

			int8 x;
			int8 y;
			int8 z;
		};

		/*int24 normals_INT24[C_MapBufferSize];
		memset(normals_INT24, 0x00, sizeof(int24) * C_MapBufferSize);
		int24* t_normals_INT24 = normals_INT24;*/

		glm::vec3 tempNormals[C_MapBufferSize];
		glm::vec3* ttn = tempNormals;

		for (int j = 0; j < 17; j++)
		{
			for (uint32 i = 0; i < ((j % 2) ? 8 : 9); i++)
			{
				int24 nor;
				m_Bytes->readBytes(&nor, sizeof(int24));

				*ttn++ = glm::vec3(-(float)nor.y / 127.0f, (float)nor.z / 127.0f, -(float)nor.x / 127.0f);
				//*t_normals_INT24++ = nor;
			}
		}

		//normalsBuffer = _Render->r.createVertexBuffer(C_MapBufferSize * sizeof(vec3), tempNormals, false);
		//normalsBuffer = _Render->r.createVertexBuffer(C_MapBufferSize * sizeof(int24), normals_INT24, false);
		normalsBuffer = m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(tempNormals, C_MapBufferSize);
	}

	// Heights
	m_Bytes->seek(startPos + header.ofsHeight);
	{
		float heights[C_MapBufferSize];
		float* t_heights = heights;

		glm::vec3 tempVertexes[C_MapBufferSize];
		glm::vec3* ttv = tempVertexes;

		BoundingBox bbox = GetColliderComponent()->GetBounds();


		float minHeight = Math::MaxFloat;
		float maxHeight = Math::MinFloat;

		for (uint32 j = 0; j < 17; j++)
		{
			for (uint32 i = 0; i < ((j % 2) ? 8 : 9); i++)
			{
				float h;
				m_Bytes->readBytes(&h, sizeof(float));

				float xpos = i * C_UnitSize;
				float zpos = j * 0.5f * C_UnitSize;
				if (j % 2)
				{
					xpos += C_UnitSize * 0.5f;
				}

				glm::vec3 v(xpos, h, zpos);
				*ttv++ = v;

				minHeight = std::min(h, minHeight);
				maxHeight = std::max(h, maxHeight);
			}
		}

		bbox.setMinY(minHeight);
		bbox.setMaxY(maxHeight);
		bbox.calculateCenter();
        GetColliderComponent()->SetBounds(bbox);

		verticesBuffer = m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(tempVertexes, C_MapBufferSize);
	}

	if (m_MapTile.GetState() != ILoadable::ELoadableState::Loaded)
		return false;

	// Textures
	ADT_MCNK_MCLY mcly[4];
	m_Bytes->seek(startPos + header.ofsLayer);
	{
		for (uint32 i = 0; i < header.nLayers; i++)
		{
			m_Bytes->readBytes(&mcly[i], sizeof(ADT_MCNK_MCLY));

			//m_DiffuseTextures[i] = m_MapTile.m_Textures.at(mcly[i].textureIndex)->diffuseTexture;
			//m_SpecularTextures[i] = m_MapTile.m_Textures.at(mcly[i].textureIndex)->specularTexture;
		}
	}

	// R, G, B - alphas, A - shadow
	std::shared_ptr<CImageBase> blendBuff = std::make_shared<CImageBase>(64, 64, 32, true);

	// Shadows
	if (header.flags.has_mcsh)
	{
		m_Bytes->seek(startPos + header.ofsShadow);
		{
			uint8 sbuf[64 * 64];
			uint8* p;
			uint8 c[8];
			p = sbuf;
			for (int j = 0; j < 64; j++)
			{
				m_Bytes->readBytes(c, 8);
				for (int i = 0; i < 8; i++)
				{
					for (int b = 0x01; b != 0x100; b <<= 1)
					{
						*p++ = (c[i] & b) ? 85 : 0;
					}
				}
			}

			for (int p = 0; p < 64 * 64; p++)
			{
				blendBuff->GetDataEx()[p * 4 + 3] = sbuf[p];
			}
		}
	}

	// Alpha
	m_Bytes->seek(startPos + header.ofsAlpha);
	{
		for (uint32 i = 1; i < header.nLayers; i++)
		{
			uint8 amap[64 * 64];
			memset(amap, 0x00, 64 * 64);

			const uint8* abuf = m_Bytes->getDataFromCurrent() + mcly[i].offsetInMCAL;

			if (mcly[i].flags.alpha_map_compressed) // Compressed: MPHD is only about bit depth!
			{
				// compressed
				const uint8* input = abuf;

				for (uint16 offset_output = 0; offset_output < 4096;)
				{
					const bool fill = *input & 0x80;
					const uint16 n = *input & 0x7F;
					++input;

					if (fill)
					{
						memset(&amap[offset_output], *input, n);
						++input;
					}
					else
					{
						memcpy(&amap[offset_output], input, n);
						input += n;
					}

					offset_output += n;
				}
			}
			else if (m_Map.isUncompressedAlpha()) // Uncomressed (4096)
			{
				std::memcpy(amap, abuf, 64 * 64);
			}
			else
			{
				uint8* p;
				p = amap;
				for (uint8 j = 0; j < 64; j++)
				{
					for (uint8 i = 0; i < 32; i++)
					{
						uint8 c = *abuf++;
						*p++ = (c & 0x0f) << 4;
						*p++ = (c & 0xf0);
					}

				}
			}

			if (!header.flags.do_not_fix_alpha_map)
			{
				for (uint8 i = 0; i < 64; ++i)
				{
					amap[i * 64 + 63] = amap[i * 64 + 62];
					amap[63 * 64 + i] = amap[62 * 64 + i];
				}
				amap[63 * 64 + 63] = amap[62 * 64 + 62];
			}

			for (int p = 0; p < 64 * 64; p++)
			{
				blendBuff->GetDataEx()[p * 4 + (i - 1)] = amap[p];
			}
		}
	}

	// Liquids
	m_Bytes->seek(startPos + header.ofsLiquid);
	{
		if (header.sizeLiquid > 8)
		{
			CRange height;
			m_Bytes->read(&height);


			// Set this chunk BBOX
			{
				BoundingBox bbox = GetColliderComponent()->GetBounds();
				float bboxMinHeight = std::min(bbox.getMin().y, (height.min - GetTranslation().y));
				float bboxMaxHeight = std::max(bbox.getMax().y, (height.max - GetTranslation().y));
				bbox.setMinY(bboxMinHeight);
				bbox.setMaxY(bboxMaxHeight);
				bbox.calculateCenter();
				GetColliderComponent()->SetBounds(bbox);
			}

			CMapChunkLiquid liquidObject(m_RenderDevice, m_Bytes, header);

			auto liquidInstance = CreateSceneNode<Liquid_Instance>();
			liquidObject.CreateInsances(liquidInstance);

			// Transform
			liquidInstance->SetTranslate(glm::vec3(0.0f, (-GetTranslation().y), 0.0f));

			// IColliderComponent3D
			{
				BoundingBox bbox = GetColliderComponent()->GetBounds();
				bbox.setMinY(height.min);
				bbox.setMaxY(height.max);
				bbox.calculateCenter();
				liquidInstance->GetColliderComponent()->SetBounds(bbox);
				liquidInstance->GetColliderComponent()->SetDebugDrawMode(false);
			}
		}
	}

	m_Bytes.reset();

	// All chunk is holes
	if (header.holes == UINT16_MAX)
		return true;

	// Material
	std::shared_ptr<ADT_MCNK_Material> mat = std::make_shared<ADT_MCNK_Material>(m_RenderDevice);

#if 0
	// Create chunk texture
	{
		// 1. Resize blend texture
		blendBuff.Resize(256, 256);

		// 2. Create result texture
		CImageBase resultTexture(256, 256, 32, false);

		if (m_MapTile.GetState() != ILoadable::ELoadableState::Loaded) return false;

		auto image = m_MapTile.m_Textures.at(mcly[0].textureIndex)->diffuseTexture;
		if (image->GetWidth() != 256 && image->GetHeight() != 256)
			image->Resize(256, 256);


		for (size_t t = 0; t < 256 * 256 * 4; t += 4)
		{
			resultTexture.GetDataEx()[t + 0] = image->GetData()[t + 0];
			resultTexture.GetDataEx()[t + 1] = image->GetData()[t + 1];
			resultTexture.GetDataEx()[t + 2] = image->GetData()[t + 2];
			resultTexture.GetDataEx()[t + 3] = 255;
		}

		for (size_t l = 1; l < header.nLayers; l++)
		{
			if (m_MapTile.GetState() != ILoadable::ELoadableState::Loaded) return false;

			auto image = m_MapTile.m_Textures.at(mcly[l].textureIndex)->diffuseTexture;
			if (image->GetWidth() != 256 && image->GetHeight() != 256)
				image->Resize(256, 256);

			for (size_t t = 0; t < 256 * 256 * 4; t += 4)
			{
				resultTexture.GetDataEx()[t + 0] = lerpUInt8(resultTexture.GetData()[t + 0], image->GetData()[t + 0], blendBuff.GetData()[t + l - 1]);
				resultTexture.GetDataEx()[t + 1] = lerpUInt8(resultTexture.GetData()[t + 1], image->GetData()[t + 1], blendBuff.GetData()[t + l - 1]);
				resultTexture.GetDataEx()[t + 2] = lerpUInt8(resultTexture.GetData()[t + 2], image->GetData()[t + 2], blendBuff.GetData()[t + l - 1]);
			}
		}

		std::shared_ptr<ITexture> diffuseTexture = m_RenderDevice.GetObjectsFactory().CreateEmptyTexture();
		diffuseTexture->LoadTextureCustom(256, 256, resultTexture.GetDataEx());
		mat->SetTexture(0, diffuseTexture);
	}
#else
	for (uint32 i = 0; i < header.nLayers; i++)
	{
		mat->SetTexture(i + 0, m_MapTile.m_Textures.at(mcly[i].textureIndex)->diffuseTexture);
		//mat->SetTexture(i + 5, m_MapTile.m_Textures.at(mcly[i].textureIndex)->specularTexture);
	}

	std::shared_ptr<ITexture> blendRBGShadowATexture = m_RenderDevice.GetObjectsFactory().CreateEmptyTexture();
	blendRBGShadowATexture->LoadTextureFromImage(blendBuff);

	mat->SetTexture(4, blendRBGShadowATexture);
	mat->SetLayersCnt(header.nLayers);
#endif

	mat->SetShadowMapExists(header.flags.has_mcsh == 1);

	{ // Geom High
		const std::vector<uint16>& mapArrayHigh = _MapShared->GenarateHighMapArray(header.holes);
		std::shared_ptr<IBuffer> __ibHigh = m_RenderDevice.GetObjectsFactory().CreateIndexBuffer(mapArrayHigh);

		std::shared_ptr<IGeometry> defaultGeometry = m_RenderDevice.GetObjectsFactory().CreateGeometry();
		defaultGeometry->AddVertexBuffer(BufferBinding("POSITION", 0), verticesBuffer);
		defaultGeometry->AddVertexBuffer(BufferBinding("NORMAL", 0), normalsBuffer);
		defaultGeometry->AddVertexBuffer(BufferBinding("TEXCOORD", 0), _MapShared->BufferTextureCoordDetailAndAlpha);
		defaultGeometry->SetIndexBuffer(__ibHigh);

		std::shared_ptr<IModel> model = m_RenderDevice.GetObjectsFactory().CreateModel();
		model->AddConnection(mat, defaultGeometry);

		GetComponent<CModelsComponent3D>()->AddModel(model);
	}


	/*{ // Geom Default
		std::vector<uint16>& mapArrayDefault = _MapShared->GenarateDefaultMapArray(header.holes);
		std::shared_ptr<IBuffer> __ibDefault = GetManager<IRenderDevice>(GetBaseManager())->CreateIndexBuffer(mapArrayDefault);

		__geomDefault = GetManager<IRenderDevice>(GetBaseManager())->CreateMesh();
		__geomDefault->AddVertexBuffer(BufferBinding("POSITION", 0), verticesBuffer);
		__geomDefault->AddVertexBuffer(BufferBinding("NORMAL", 0), normalsBuffer);
		__geomDefault->AddVertexBuffer(BufferBinding("TEXCOORD", 0), _MapShared->BufferTextureCoordDetail);
		__geomDefault->AddVertexBuffer(BufferBinding("TEXCOORD", 1), _MapShared->BufferTextureCoordAlpha);
		__geomDefault->SetIndexBuffer(__ibDefault);
		__geomDefault->SetMaterial(mat);
		__geomDefault->SetType(SN_TYPE_ADT_CHUNK);

		AddMesh(__geomDefault);
	}*/

	return true;
}

bool CMapChunk::Delete()
{
	return true;
}

//

/*void CMapChunk::drawPass(int anim) VERY OLD :)
{
	if (anim)
	{
		glActiveTexture(GL_TEXTURE0);
		glMatrixMode(GL_TEXTURE);
		glPushMatrix();

		// note: this is ad hoc and probably completely wrong
		int spd = (anim & 0x08) | ((anim & 0x10) >> 2) | ((anim & 0x20) >> 4) | ((anim & 0x40) >> 6);
		int dir = anim & 0x07;
		const float texanimxtab[8] = {0, 1, 1, 1, 0, -1, -1, -1};
		const float texanimytab[8] = {1, 1, 0, -1, -1, -1, 0, 1};
		float fdx = -texanimxtab[dir], fdy = texanimytab[dir];

		int animspd = (int)(200.0f * C_DetailSize);
		float f = (((int)(_TimeManager->animtime * (spd / 15.0f))) % animspd) / (float)animspd;
		glTranslatef(f * fdx, f * fdy, 0);
	}

	glDrawElements(GL_TRIANGLE_STRIP, striplen, GL_UNSIGNED_SHORT, m_Indexes);

	if (anim)
	{
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glActiveTexture(GL_TEXTURE1);
	}
}*/
