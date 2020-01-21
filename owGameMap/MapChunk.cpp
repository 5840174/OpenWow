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

CMapChunk::CMapChunk(CMap* _mapController, std::weak_ptr<CMapTile> _parentTile) 
	: m_MapController(_mapController)
	, m_ParentADT(_parentTile)
{}

CMapChunk::~CMapChunk()
{
}

void CMapChunk::Initialize()
{
}

void CMapChunk::Initialize(const std::string & _fileName, const ADT_MCIN & _mcin)
{
    m_FileName = _fileName;
    mcin = _mcin;

    memset(mcly, 0x00, sizeof(ADT_MCNK_MCLY) * 4);
}

uint32 CMapChunk::GetAreaID() const
{
    return header.areaid;
}


//
// ISceneNode
//

bool CMapChunk::Accept(IVisitor* visitor)
{
	//AbstractPass* visitorAsBasePass = dynamic_cast<AbstractPass*>(visitor);
	//const ICamera* camera = visitorAsBasePass->GetRenderEventArgs()->Camera;

    /*if (!GetComponent<CColliderComponent3D>()->CheckDistance(camera, m_QualitySettings->ADT_MCNK_Distance))
    {
        return false;
    }*/

	return SceneNode3D::Accept(visitor);
}


bool CMapChunk::PreLoad()
{
	m_File = GetBaseManager()->GetManager<IFilesManager>()->Open(m_FileName);
	if (m_File == nullptr)
		return false;

	m_File->seek(mcin.offset);

	// Chunk + size (8)
	m_File->seekRelative(4); // MCNK
	uint32_t size;
	m_File->readBytes(&size, sizeof(uint32_t));
	_ASSERT(size + 8 == mcin.size);

	uint32_t startPos = m_File->getPos();

	// Read header
	m_File->readBytes(&header, sizeof(ADT_MCNK_Header));


	// Scene node params
    {
        // Set translate
        SetTranslate(vec3(header.xpos * (-1.0f) + C_ZeroPoint, header.ypos, header.zpos * (-1.0f) + C_ZeroPoint));
    }

    {
        vec3 translate = GetTranslation();

        // Bounds
        BoundingBox bbox
        (
            vec3(translate.x, Math::MaxFloat, translate.z),
            vec3(translate.x + C_ChunkSize, Math::MinFloat, translate.z + C_ChunkSize)
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
		normalsBuffer = GetBaseManager()->GetManager<IRenderDevice>()->CreateVertexBuffer(tempNormals, C_MapBufferSize);
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

		verticesBuffer = GetBaseManager()->GetManager<IRenderDevice>()->CreateVertexBuffer(tempVertexes, C_MapBufferSize);
	}

	// Textures
	m_File->seek(startPos + header.ofsLayer);
	{
		std::shared_ptr<CMapTile> parentADT = m_ParentADT.lock();
		_ASSERT(parentADT != NULL);

		for (uint32 i = 0; i < header.nLayers; i++)
		{
			m_File->readBytes(&mcly[i], sizeof(ADT_MCNK_MCLY));
			//m_DiffuseTextures[i] = parentADT->m_Textures.at(i)->diffuseTexture;
			//m_SpecularTextures[i] = parentADT->m_Textures.at(i)->specularTexture;
			m_DiffuseTextures[i] = parentADT->m_Textures.at(mcly[i].textureIndex)->diffuseTexture;
			m_SpecularTextures[i] = parentADT->m_Textures.at(mcly[i].textureIndex)->specularTexture;
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
		CMap* mapController = m_MapController;
		_ASSERT(mapController != NULL);

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
			else if (mapController->isUncompressedAlpha()) // Uncomressed (4096)
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

	m_BlendRBGShadowATexture = GetBaseManager()->GetManager<IRenderDevice>()->CreateTexture();
	m_BlendRBGShadowATexture->LoadTextureCustom(64, 64, blendbuf);

	// Liquids
	m_File->seek(startPos + header.ofsLiquid);
	{
		if (header.sizeLiquid > 8)
		{
			CRange height;
			m_File->readBytes(&height, 8);

			std::shared_ptr<CADT_Liquid> m_Liquid = std::make_shared<CADT_Liquid>(GetBaseManager(), 8, 8);
			m_Liquid->CreateFromMCLQ(m_File, header);

            vec3 position = vec3(0.0f, - GetTranslation().y, 0.0f);

			std::shared_ptr<Liquid_Instance> liq = CreateSceneNode<Liquid_Instance>();
			liq->Initialize(m_Liquid, position);
		}
	}

	m_File.reset();

	// All chunk is holes
	if (header.holes == UINT16_MAX)
		return true;

	// Material
	std::shared_ptr<ADT_MCNK_Material> mat = std::make_shared<ADT_MCNK_Material>(GetBaseManager()->GetManager<IRenderDevice>(), m_ParentADT);
	mat->SetWrapper(mat);
	for (uint32 i = 0; i < header.nLayers; i++)
	{
		mat->SetTexture(i, m_DiffuseTextures[i]); // DXT1
		//mat->SetTexture(i + 5, m_SpecularTextures[i]); // DXT1
	}
	mat->SetTexture(4, m_BlendRBGShadowATexture);
	mat->SetLayersCnt(header.nLayers);
	mat->SetShadowMapExists(header.flags.has_mcsh == 1);

	{ // Geom High
		std::vector<uint16>& mapArrayHigh = _MapShared->GenarateHighMapArray(header.holes);
		std::shared_ptr<IBuffer> __ibHigh = GetBaseManager()->GetManager<IRenderDevice>()->CreateIndexBuffer(mapArrayHigh);

		__geomDefault = GetBaseManager()->GetManager<IRenderDevice>()->CreateMesh();
		__geomDefault->AddVertexBuffer(BufferBinding("POSITION", 0), verticesBuffer);
		__geomDefault->AddVertexBuffer(BufferBinding("NORMAL", 0), normalsBuffer);
		__geomDefault->AddVertexBuffer(BufferBinding("TEXCOORD", 0), _MapShared->BufferTextureCoordDetail);
		__geomDefault->AddVertexBuffer(BufferBinding("TEXCOORD", 1), _MapShared->BufferTextureCoordAlpha);
		__geomDefault->SetIndexBuffer(__ibHigh);
		__geomDefault->SetMaterial(mat);

		GetComponent<CMeshComponent3D>()->AddMesh(__geomDefault);
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
