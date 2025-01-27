#include "stdafx.h"

// Include
#include "MapTile.h"
#include "Map.h"

// General
#include "MapChunk.h"

// Additional
#include "MapHelpers.h"
#include "MapChunkLiquid.h"
#include "MapChunkMaterial.h"
#include "Liquid/LiquidBaseInstance.h"

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

	float CalculateBarryCentic(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec2& pos)
	{
		float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);

		float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
		float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
		float l3 = 1.0f - l1 - l2;

		return (l1 * p1.y) + (l2 * p2.y) + (l3 * p3.y);
	}
}

CMapChunk::CMapChunk(IScene& Scene, CMapTile& MapTileParent, const SMapTile_MCIN& Chunk, const std::shared_ptr<IByteBuffer>& Bytes)
	: CSceneNode(Scene)
	, m_MapTile(MapTileParent)
	, m_Map(m_MapTile.GetMap())
{
	m_Bytes = MakeShared(CByteBuffer, Bytes->getData() + Chunk.offset, Chunk.size);
	SetName("MapTile[" + std::to_string(m_MapTile.GetTileIndexX()) + "," + std::to_string(m_MapTile.GetTileIndexZ()) + "]_Chunk[" + std::to_string(Chunk.offset) + "]");
	SetUpdateEnabled(false);
}

CMapChunk::~CMapChunk()
{}



//
// CMapChunk
//
uint32 CMapChunk::GetAreaID() const
{
    return m_Header.areaid;
}

float CMapChunk::GetTerrainHeight(glm::vec3 PositionInChunk) const
{
	// Index of Unit
	int16 indexX = glm::round(PositionInChunk.x / float(C_UnitSize) - 0.5f);
	if (indexX < 0) indexX = 0;
	int16 indexZ = glm::round(PositionInChunk.z / float(C_UnitSize) - 0.5f);
	if (indexZ < 0) indexZ = 0;

	if (IsHole(m_Header.holes, indexX / 2, indexZ / 2))
		return Math::MaxFloat;

	const uint16 indexIntoArray0 = GetOuterMapChunkArrayIndex(indexZ,     indexX    );
	const uint16 indexIntoArray1 = GetOuterMapChunkArrayIndex(indexZ + 1, indexX    );
	const uint16 indexIntoArray2 = GetOuterMapChunkArrayIndex(indexZ,     indexX + 1);
	const uint16 indexIntoArray3 = GetOuterMapChunkArrayIndex(indexZ + 1, indexX + 1);

	const float modX = glm::mod(PositionInChunk.x, C_UnitSize);
	const float modZ = glm::mod(PositionInChunk.z, C_UnitSize);
	if (modZ < (C_UnitSize - modX))
	{
		return CalculateBarryCentic(
			glm::vec3(0.0f,       m_Heights[indexIntoArray0], 0.0f      ),
			glm::vec3(C_UnitSize, m_Heights[indexIntoArray1], 0.0f      ),
			glm::vec3(0.0f,       m_Heights[indexIntoArray2], C_UnitSize),
			glm::vec2(modZ, modX)
		);
	}
	else
	{
		return CalculateBarryCentic(
			glm::vec3(0.0f,       m_Heights[indexIntoArray2], C_UnitSize),
			glm::vec3(C_UnitSize, m_Heights[indexIntoArray1], 0.0f      ),
			glm::vec3(C_UnitSize, m_Heights[indexIntoArray3], C_UnitSize),
			glm::vec2(modZ, modX)
		);
	}
}

void CMapChunk::ExtendMapChunkBounds(const BoundingBox & OtherBBox)
{
	ExtendBounds(OtherBBox);
}



//
// ISceneNode
//
void CMapChunk::Initialize()
{
	__super::Initialize();

	// Chunk + size (8)
	uint32_t offset;
	m_Bytes->read(&offset);
	uint32_t size;
	m_Bytes->read(&size);

	// Read header
	m_Bytes->readBytes(&m_Header, sizeof(SMapChunk_MCNK));



	BoundingBox bbox
	(
		glm::vec3(- m_Header.xpos + C_ZeroPoint,               Math::MaxFloat, - m_Header.zpos + C_ZeroPoint),
		glm::vec3(- m_Header.xpos + C_ZeroPoint + C_ChunkSize, Math::MinFloat, - m_Header.zpos + C_ZeroPoint + C_ChunkSize)
	);

	SetCullStrategy(ECullStrategy::ByFrustrumAndDistance2D);
	SetCullDistance(GetBaseManager().GetManager<ISettings>()->GetGroup("WoWSettings")->GetPropertyT<float>("MapChunkRenderDistance")->Get());
	SetBounds(bbox);
	SetDebugDrawMode(false);
	SetDebugDrawColor(ColorRGBA(0.3f, 1.0f, 0.2f, 0.8f));
}

//
// ILoadable
//
bool CMapChunk::Load()
{
	uint32_t startPos = m_Bytes->getPos() - sizeof(SMapChunk_MCNK);

	std::shared_ptr<IBuffer> verticesBuffer = nullptr;
	std::shared_ptr<IBuffer> normalsBuffer = nullptr;
	std::shared_ptr<IBuffer> mccvBuffer = nullptr;

	// Normals
	m_Bytes->seek(startPos + m_Header.ofsNormal);
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

		glm::vec3 tempNormals[C_MapBufferSize];
		glm::vec3* ttn = tempNormals;

		for (int j = 0; j < 17; j++)
		{
			for (uint32 i = 0; i < ((j % 2) ? 8 : 9); i++)
			{
				int24 nor;
				m_Bytes->readBytes(&nor, sizeof(int24));

				*ttn++ = glm::vec3((-1.0f) * (float)nor.y / 127.0f, (float)nor.z / 127.0f, (-1.0f) * (float)nor.x / 127.0f);
				//*t_normals_INT24++ = nor;
			}
		}

		normalsBuffer = GetRenderDevice().GetObjectsFactory().CreateVertexBuffer(tempNormals, C_MapBufferSize);
	}

	// Heights
	m_Bytes->seek(startPos + m_Header.ofsHeight);
	{
		glm::vec3 vertices[C_MapBufferSize];
		glm::vec3* ttv = vertices;

		float minHeight = Math::MaxFloat;
		float maxHeight = Math::MinFloat;

		for (uint32 j = 0; j < 17; j++)
		{
			for (uint32 i = 0; i < ((j % 2) ? 8 : 9); i++)
			{
				float height;
				m_Bytes->read(&height);

				float xpos = i * C_UnitSize;
				float zpos = j * 0.5f * C_UnitSize;
				if (j % 2)
					xpos += C_UnitSize * 0.5f;

				glm::vec3 v(
					(-1.0f) * m_Header.xpos + C_ZeroPoint + xpos, 
					          m_Header.ypos + height,
					(-1.0f) * m_Header.zpos + C_ZeroPoint + zpos
				);
				*ttv++ = v;

				minHeight = std::min(v.y, minHeight);
				maxHeight = std::max(v.y, maxHeight);
			}
		}

		for (uint32 i = 0; i < C_MapBufferSize; i++)
			m_Heights[i] = vertices[i].y;

		// Update bounds
		{
			BoundingBox bbox = GetBounds();

			auto min = bbox.getMin();
			min.y = minHeight;
			bbox.setMin(min);

			auto max = bbox.getMax();
			max.y = maxHeight;
			bbox.setMax(max);

			SetBounds(bbox);
		}

		verticesBuffer = GetRenderDevice().GetObjectsFactory().CreateVertexBuffer(vertices, C_MapBufferSize);
	}

	_ASSERT(m_MapTile.GetState() == ILoadable::ELoadableState::Loaded);

	// Textures
	SMapChunk_MCLY mcly[4];
	m_Bytes->seek(startPos + m_Header.ofsLayer);
	{
		for (uint32 i = 0; i < m_Header.nLayers; i++)
		{
			m_Bytes->readBytes(&mcly[i], sizeof(SMapChunk_MCLY));

			//m_DiffuseTextures[i] = m_MapTile.m_Textures.at(mcly[i].textureIndex)->diffuseTexture;
			//m_SpecularTextures[i] = m_MapTile.m_Textures.at(mcly[i].textureIndex)->specularTexture;
		}
	}

	// R, G, B - alphas, A - shadow
	auto blendBuff = MakeShared(CImageBase, "", 64, 64, 32, true);

	// Shadows (MCSH)
	if (m_Header.flags.has_mcsh)
	{
		m_Bytes->seek(startPos + m_Header.ofsShadow);
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

			// TODO: Investiagete it
			/*if (false == m_Header.flags.do_not_fix_alpha_map)
			{
				for (uint8 i = 0; i < 64; ++i)
				{
					sbuf[i * 64 + 63] = sbuf[i * 64 + 62];
					sbuf[63 * 64 + i] = sbuf[62 * 64 + i];
				}
				sbuf[63 * 64 + 63] = sbuf[62 * 64 + 62];
			}*/

			for (int p = 0; p < 64 * 64; p++)
			{
				blendBuff->GetDataEx()[p * 4 + 3] = sbuf[p];
			}
		}
	}

	// Alpha (MCAL)
	m_Bytes->seek(startPos + m_Header.ofsAlpha);
	{
		for (uint32 i = 1; i < m_Header.nLayers; i++)
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
				uint8* p = amap;
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

			if (false == m_Header.flags.do_not_fix_alpha_map)
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

	// Liquids (MCLQ)
	m_Bytes->seek(startPos + m_Header.ofsLiquid);
	{
		if (m_Header.sizeLiquid > 8)
		{
			auto liquidInstance = CreateSceneNode<CLiquidBaseInstance>();

			CMapChunkLiquid liquidObject(GetRenderDevice(), m_Bytes, m_Header);
			liquidObject.CreateInsances(liquidInstance);

			// Transform
			liquidInstance->SetLocalPosition(glm::vec3((-1.0f) * m_Header.xpos + C_ZeroPoint, 0.0f, (-1.0f) * m_Header.zpos + C_ZeroPoint));

			// Collider
			BoundingBox bbox(
				glm::vec3(0.0f,        liquidObject.getMinHeight() - 1.0f, 0.0f),
				glm::vec3(C_ChunkSize, liquidObject.getMaxHeight() + 1.0f, C_ChunkSize)
			);
			liquidInstance->SetBounds(bbox);

			ExtendMapChunkBounds(liquidInstance->GetWorldBounds());
		}
	}

	// Vertex colors (MCCV)
	if (m_Header.flags.has_mccv)
	{
		m_Bytes->seek(startPos + m_Header.ofsMCCV);
		{
			glm::vec3 mccvColorsVec3[C_MapBufferSize] = {};

			uint32 mccvColorsUINT8[C_MapBufferSize];
			memset(mccvColorsUINT8, 0x00, sizeof(uint32) * C_MapBufferSize);

			glm::vec3* t_mccvColorsVec3 = mccvColorsVec3;
			uint32* t_mccvColorsUINT8 = mccvColorsUINT8;

			for (int j = 0; j < 17; j++)
			{
				for (uint32 i = 0; i < ((j % 2u) ? 8u : 9u); i++)
				{
					CBgra nor;
					m_Bytes->readBytes(&nor, sizeof(CBgra));

					*t_mccvColorsVec3++ = glm::vec3(nor.r, nor.g, nor.b) / 127.0f;

					//*t_mccvColorsUINT8++ = uint32(
					//	(uint8)(nor[3]) << 24 |
					//	(uint8)(nor[0]) << 16 |
					//	(uint8)(nor[1]) << 8 |
					//	(uint8)(nor[2])
					//);
				}
			}

			mccvBuffer = GetRenderDevice().GetObjectsFactory().CreateVertexBuffer(mccvColorsVec3, C_MapBufferSize);
		}
	}

	m_Bytes.reset();


	// Update map tile bounds
	m_MapTile.ExtendMapTileBounds(GetBounds());

	// All chunk is holes
	if (m_Header.holes == UINT16_MAX)
		return true;

	// Material
	std::shared_ptr<CMapChunkMaterial> mapChunkMaterial = MakeShared(CMapChunkMaterial, GetRenderDevice());

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

		for (size_t l = 1; l < m_Header.nLayers; l++)
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
	for (uint32 i = 0; i < m_Header.nLayers; i++)
	{
		mapChunkMaterial->SetTexture(i + 0, m_MapTile.GetTextureInfo(mcly[i].textureIndex)->diffuseTexture);
		if (false == mcly->flags.use_cube_map_reflection)
			mapChunkMaterial->SetTexture(i + 5, m_MapTile.GetTextureInfo(mcly[i].textureIndex)->specularTexture);
	}

	auto blendRBGShadowATexture = GetRenderDevice().GetObjectsFactory().CreateEmptyTexture();
	blendRBGShadowATexture->LoadTexture2DFromImage(blendBuff);

	mapChunkMaterial->SetTexture(4, blendRBGShadowATexture);
	mapChunkMaterial->SetLayersCnt(m_Header.nLayers);
#endif

	mapChunkMaterial->SetIsShadowMapExists(m_Header.flags.has_mcsh);
	mapChunkMaterial->SetIsMCCVExists(m_Header.flags.has_mccv);
	mapChunkMaterial->SetIsNortrend(m_Map.IsNortrend());

	// Geom High
	/*{ 
		const std::vector<uint16>& mapArrayHigh = m_Map.GenarateHighMapArray(m_Header.holes);
		std::shared_ptr<IBuffer> indexBufferHigh = GetRenderDevice().GetObjectsFactory().CreateIndexBuffer(mapArrayHigh);

		std::shared_ptr<IGeometry> defaultGeometry = GetRenderDevice().GetObjectsFactory().CreateGeometry();
		defaultGeometry->AddVertexBuffer(BufferBinding("POSITION", 0), verticesBuffer);
		defaultGeometry->AddVertexBuffer(BufferBinding("NORMAL", 0), normalsBuffer);
		defaultGeometry->AddVertexBuffer(BufferBinding("TEXCOORD", 0), m_Map.GetBufferTextureCoordDetailAndAlpha());
		if (mccvBuffer)
			defaultGeometry->AddVertexBuffer(BufferBinding("COLOR", 0), mccvBuffer);

		defaultGeometry->SetIndexBuffer(indexBufferHigh);

		auto mapChunkModel = GetRenderDevice().GetObjectsFactory().CreateModel();
		mapChunkModel->AddConnection(mapChunkMaterial, defaultGeometry);

		GetComponentT<IModelComponent>()->SetModel(mapChunkModel);
	}*/

	// Geom Default
	{
		const std::vector<uint16>& mapArrayDefault = m_Map.GenarateDefaultMapArray(m_Header.holes);
		std::shared_ptr<IBuffer> indexBufferDefault = GetRenderDevice().GetObjectsFactory().CreateIndexBuffer(mapArrayDefault);

		auto defaultGeometry = GetRenderDevice().GetObjectsFactory().CreateGeometry();
		defaultGeometry->AddVertexBuffer(BufferBinding("POSITION", 0), verticesBuffer);
		defaultGeometry->AddVertexBuffer(BufferBinding("NORMAL", 0), normalsBuffer);
		defaultGeometry->AddVertexBuffer(BufferBinding("TEXCOORD", 0), m_Map.GetBufferTextureCoordDetailAndAlpha());
		if (mccvBuffer)
			defaultGeometry->AddVertexBuffer(BufferBinding("COLOR", 0), mccvBuffer);

		defaultGeometry->SetIndexBuffer(indexBufferDefault);

		auto mapChunkModel = GetRenderDevice().GetObjectsFactory().CreateModel();
		mapChunkModel->AddConnection(mapChunkMaterial, defaultGeometry);

		GetComponentT<IModelComponent>()->SetModel(mapChunkModel);
	}

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
		float f = (((int)(_TimeManager->m_AnimTime * (spd / 15.0f))) % animspd) / (float)animspd;
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
