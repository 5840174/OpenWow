#include "stdafx.h"

// Include
#include "MapTile.h"
#include "Map.h"

// General
#include "MapChunk.h"

// Additional
#include "MapLiquid.h"
#include "Map_Shared.h"
#include "MapChunkMaterial.h"

CMapChunk::CMapChunk(IRenderDevice& RenderDevice, const CMap& Map, const CMapTile& MapTile, const std::shared_ptr<IByteBuffer>& Bytes)
	: m_RenderDevice(RenderDevice)
	, m_Map(Map)
	, m_MapTile(MapTile)
	, m_File(Bytes)
{}

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
}

std::string CMapChunk::GetName() const
{
	return "MapChunk " + 2/*std::to_string(m_MCIN.offset)*/;
}

void CMapChunk::Accept(IVisitor* visitor)
{
	//AbstractPass* visitorAsBasePass = dynamic_cast<AbstractPass*>(visitor);
	//const ICamera* camera = visitorAsBasePass->GetRenderEventArgs()->Camera;

    /*if (!GetComponent<CColliderComponent3D>()->CheckDistance(camera, m_QualitySettings->ADT_MCNK_Distance))
    {
        return false;
    }*/

	SceneNode3D::Accept(visitor);
}


bool CMapChunk::PreLoad()
{
	//m_File->seek(m_MCIN.offset);

	// Chunk + size (8)
	uint32_t offset;
	m_File->read(&offset);

	uint32_t size;
	m_File->read(&size);

	uint32_t startPos = m_File->getPos();

	// Read header
	m_File->readBytes(&header, sizeof(ADT_MCNK_Header));

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
            glm::vec3(translate.x,               Math::MaxFloat, translate.z),
			glm::vec3(translate.x + C_ChunkSize, Math::MinFloat, translate.z + C_ChunkSize)
        );

		GetComponent<CColliderComponent3D>()->SetBounds(bbox);
	}

	return true;
}

bool CMapChunk::Load()
{
	uint32_t startPos = m_File->getPos() - sizeof(ADT_MCNK_Header);

	std::shared_ptr<IBuffer> verticesBuffer = nullptr;
	std::shared_ptr<IBuffer> normalsBuffer = nullptr;

	uint8 blendbuf[64 * 64 * 4];
	memset(blendbuf, 0, 64 * 64 * 4);

	// Normals
	m_File->seek(startPos + header.ofsNormal);
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

		vec3 tempNormals[C_MapBufferSize];
		vec3* ttn = tempNormals;

		for (int j = 0; j < 17; j++)
		{
			for (uint32 i = 0; i < ((j % 2) ? 8 : 9); i++)
			{
				int24 nor;
				m_File->readBytes(&nor, sizeof(int24));

				*ttn++ = vec3(-(float)nor.y / 127.0f, (float)nor.z / 127.0f, -(float)nor.x / 127.0f);
				//*t_normals_INT24++ = nor;
			}
		}

		//normalsBuffer = _Render->r.createVertexBuffer(C_MapBufferSize * sizeof(vec3), tempNormals, false);
		//normalsBuffer = _Render->r.createVertexBuffer(C_MapBufferSize * sizeof(int24), normals_INT24, false);
		normalsBuffer = m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(tempNormals, C_MapBufferSize);
	}

	// Heights
	m_File->seek(startPos + header.ofsHeight);
	{
		float heights[C_MapBufferSize];
		float* t_heights = heights;

		vec3 tempVertexes[C_MapBufferSize];
		vec3* ttv = tempVertexes;

		BoundingBox bbox = GetComponent<CColliderComponent3D>()->GetBounds();


		float minHeight = Math::MaxFloat;
		float maxHeight = Math::MinFloat;

		for (uint32 j = 0; j < 17; j++)
		{
			for (uint32 i = 0; i < ((j % 2) ? 8 : 9); i++)
			{
				float h;
				m_File->readBytes(&h, sizeof(float));

				float xpos = i * C_UnitSize;
				float zpos = j * 0.5f * C_UnitSize;
				if (j % 2)
				{
					xpos += C_UnitSize * 0.5f;
				}

				vec3 v = vec3(xpos, h, zpos);
				*ttv++ = v;

				minHeight = std::min(h, minHeight);
				maxHeight = std::max(h, maxHeight);
			}
		}

		bbox.setMinY(minHeight + header.ypos);
		bbox.setMaxY(maxHeight + header.ypos);
		bbox.calculateCenter();
        GetComponent<CColliderComponent3D>()->SetBounds(bbox);

		verticesBuffer = m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(tempVertexes, C_MapBufferSize);
	}

	// Textures
	ADT_MCNK_MCLY mcly[4];
	std::shared_ptr<ITexture> m_DiffuseTextures[4];
	std::shared_ptr<ITexture> m_SpecularTextures[4];

	std::shared_ptr<ITexture> m_BlendRBGShadowATexture;

	m_File->seek(startPos + header.ofsLayer);
	{
		for (uint32 i = 0; i < header.nLayers; i++)
		{
			m_File->readBytes(&mcly[i], sizeof(ADT_MCNK_MCLY));
			//m_DiffuseTextures[i] = parentADT->m_Textures.at(i)->diffuseTexture;
			//m_SpecularTextures[i] = parentADT->m_Textures.at(i)->specularTexture;
			m_DiffuseTextures[i] = m_MapTile.m_Textures.at(mcly[i].textureIndex)->diffuseTexture;
			m_SpecularTextures[i] = m_MapTile.m_Textures.at(mcly[i].textureIndex)->specularTexture;
		}
	}

	// Shadows
	m_File->seek(startPos + header.ofsShadow);
	{
		uint8 sbuf[64 * 64];
		uint8* p;
		uint8 c[8];
		p = sbuf;
		for (int j = 0; j < 64; j++)
		{
			m_File->readBytes(c, 8);
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
			blendbuf[p * 4 + 3] = sbuf[p];
		}
	}

	// Alpha
	m_File->seek(startPos + header.ofsAlpha);
	{
		for (uint32 i = 1; i < header.nLayers; i++)
		{
			uint8 amap[64 * 64];
			memset(amap, 0x00, 64 * 64);
			const uint8* abuf = m_File->getDataFromCurrent() + mcly[i].offsetInMCAL;

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
				memcpy(amap, abuf, 64 * 64);
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
				blendbuf[p * 4 + (i - 1)] = amap[p];
			}
		}
	}

	m_BlendRBGShadowATexture = m_RenderDevice.GetObjectsFactory().CreateEmptyTexture();
	m_BlendRBGShadowATexture->LoadTextureCustom(64, 64, blendbuf);

	// Liquids
	m_File->seek(startPos + header.ofsLiquid);
	{
		if (header.sizeLiquid > 8)
		{
			CRange height;
			m_File->readBytes(&height, 8);

			std::shared_ptr<CADT_Liquid> m_Liquid = std::make_shared<CADT_Liquid>(m_RenderDevice, 8, 8);
			m_Liquid->CreateFromMCLQ(m_File, header);

            vec3 position = vec3(0.0f, - GetTranslation().y, 0.0f);

			Liquid_Instance* liq = CreateSceneNode<Liquid_Instance>();
			liq->LiquidInitialize(m_Liquid, position);
		}
	}

	m_File.reset();

	// All chunk is holes
	if (header.holes == UINT16_MAX)
		return true;

	// Material
	std::shared_ptr<ADT_MCNK_Material> mat = std::make_shared<ADT_MCNK_Material>(m_RenderDevice);

	for (uint32 i = 0; i < header.nLayers; i++)
	{
		mat->SetTexture(i, m_DiffuseTextures[i]); // DXT1
		//mat->SetTexture(i + 5, m_SpecularTextures[i]); // DXT1
	}
	mat->SetTexture(4, m_BlendRBGShadowATexture);
	mat->SetLayersCnt(header.nLayers);
	mat->SetShadowMapExists(header.flags.has_mcsh == 1);

	{ // Geom High
		const std::vector<uint16>& mapArrayHigh = _MapShared->GenarateHighMapArray(header.holes);
		std::shared_ptr<IBuffer> __ibHigh = m_RenderDevice.GetObjectsFactory().CreateIndexBuffer(mapArrayHigh);

		std::shared_ptr<IGeometry> defaultGeometry = m_RenderDevice.GetObjectsFactory().CreateGeometry();
		defaultGeometry->AddVertexBuffer(BufferBinding("POSITION", 0), verticesBuffer);
		defaultGeometry->AddVertexBuffer(BufferBinding("NORMAL", 0), normalsBuffer);
		defaultGeometry->AddVertexBuffer(BufferBinding("TEXCOORD", 0), _MapShared->BufferTextureCoordDetail);
		defaultGeometry->AddVertexBuffer(BufferBinding("TEXCOORD", 1), _MapShared->BufferTextureCoordAlpha);
		defaultGeometry->SetIndexBuffer(__ibHigh);

		std::shared_ptr<IModel> model = m_RenderDevice.GetObjectsFactory().CreateModel();
		model->AddConnection(mat, defaultGeometry);

		GetComponent<CMeshComponent3D>()->AddMesh(model);
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
