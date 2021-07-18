#include "stdafx.h"

// General
#include "M2.h"

CM2::CM2(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const std::string& FileName)
	: m_FileName(FileName)
	, m_UniqueName("")

	// Loops and sequences
	, m_IsAnimated(false)

	, m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
{
	// 1. Fix filename ('*.MDX' -> '*.M2')
	if (m_FileName.back() != '2')
	{
		m_FileName[m_FileName.length() - 2] = '2';
		m_FileName[m_FileName.length() - 1] = '\0';
		m_FileName.resize(m_FileName.length() - 1);
	}

	// 2. Try open file and initialize internal paths
	auto modelFile = BaseManager.GetManager<IFilesManager>()->Open(m_FileName);
	if (modelFile == nullptr)
		throw CException("M2: Model file not found '" + m_FileName + "'.");

	m_FilePath = modelFile->Path();
	m_FileNameWithoutExt = m_FileName.substr(0, m_FileName.length() - 3);

	m_Bytes = modelFile;

	// 3. Read header and important data
	m_Bytes->read(&m_Header);

	//_ASSERT(std::strcmp(m_Header.magic, "MD20") == 0);
	//_ASSERT(m_Header.version >= 256 && m_Header.version <= 257);
	//_ASSERT(m_Header.version >= 260 && m_Header.version <= 263);
	
	if (m_Header.name.size > 0)
		m_UniqueName = std::string((const char*)(m_Bytes->getData() + m_Header.name.offset));

	m_Bounds = m_Header.bounding_box.Convert();
}

CM2::~CM2()
{
}



//
// ISceneNodeProvider
//
void CM2::CreateInsances(const std::shared_ptr<ISceneNode>& Parent) const
{
	for (const auto& it : m_Skins)
	{
		Parent->GetComponentT<IModelComponent>()->AddModel(it);
		break;
	}
}



//
// ILoadable
//
bool CM2::Load()
{
	// Skeleton
	m_Skeleton = std::make_unique<CM2_Comp_Skeleton>(*this);
	m_Skeleton->Load(m_Header, m_Bytes);

	// Colors, textures and etc...
	m_Materials = std::make_unique<CM2_Comp_Materials>(*this);
	m_Materials->Load(m_Header, m_Bytes);

	// Lights, cameras, attachments
	m_Miscellaneous = std::make_unique<CM2_Comp_Miscellaneous>(*this);
	m_Miscellaneous->Load(m_Header, m_Bytes);


	// Skins
	if (m_Header.vertices.size > 0)
	{
		// Vertices
		std::vector<SM2_Vertex>	m2Vertexes;

		const SM2_Vertex* vertexes = (const SM2_Vertex*)(m_Bytes->getData() + m_Header.vertices.offset);
		for (uint32 i = 0; i < m_Header.vertices.size; i++)
		{
			SM2_Vertex vertex = vertexes[i];
			vertex.pos = Fix_From_XZmY_To_XYZ(vertex.pos);
			vertex.normal = Fix_From_XZmY_To_XYZ(vertex.normal);
			m2Vertexes.push_back(vertex);
		}

#if WOW_CLIENT_VERSION <= WOW_BC_2_4_3
		_ASSERT(m_Header.skin_profiles.size > 0);
		const SM2_SkinProfile* m2Skins = (const SM2_SkinProfile*)(m_F->getData() + m_Header.skin_profiles.offset);
		for (uint32 i = 0; i < m_Header.skin_profiles.size; i++)
		{
			std::shared_ptr<CM2_Skin> skin = std::make_shared<CM2_Skin>(m_BaseManager, m_RenderDevice, *this, m2Skins[i]);
			skin->Load(m_Header, m_F, m2Vertexes);
			m_Skins.push_back(skin);
			break;
		}
#else
		_ASSERT(m_Header.num_skin_profiles > 0);

		for (uint32 i = 0; i < m_Header.num_skin_profiles; i++)
		{
			char buf[256];
			sprintf_s(buf, "%s%02d.skin", m_FileNameWithoutExt.c_str(), i);

			std::shared_ptr<IFile> skinFile = GetBaseManager().GetManager<IFilesManager>()->Open(buf);

			const SM2_SkinProfile* m2Skin = (const SM2_SkinProfile*)skinFile->getData();

			auto skin = MakeShared(CM2_Skin, m_BaseManager, m_RenderDevice, *this, *m2Skin);
			skin->Load(m_Header, skinFile, m2Vertexes);
			m_Skins.push_back(skin);
			break;
		}
#endif
	}
	else
	{
#if WOW_CLIENT_VERSION <= WOW_BC_2_4_3
		//Log::Warn("M2[%s] don't contain geometry. Skins [%d]", getFilename().c_str(), m_Header.skin_profiles.size);
#else
		//Log::Warn("M2[%s] don't contain geometry. Skins [%d]", getFilename().c_str(), m_Header.num_skin_profiles);
#endif
	}


#if 1
	// Collisions
	std::shared_ptr<IBuffer> collisonVertexBuffer = nullptr;
	std::shared_ptr<IBuffer> collisonIndexBuffer = nullptr;

	if (m_Header.collisionVertices.size > 0)
	{
		std::vector<glm::vec3> collisionVerticesArray;
		const glm::vec3* collisionVertices = (const glm::vec3*)(m_Bytes->getData() + m_Header.collisionVertices.offset);
		for (uint32 i = 0; i < m_Header.collisionVertices.size; i++)
			collisionVerticesArray.push_back(Fix_From_XZmY_To_XYZ(collisionVertices[i]));

		collisonVertexBuffer = m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(collisionVerticesArray);
	}

	if (m_Header.collisionTriangles.size > 0)
	{
		std::vector<uint16> collisionTrianglesArray;
		const uint16* collisionTriangles = (const uint16*)(m_Bytes->getData() + m_Header.collisionTriangles.offset);
		for (uint32 i = 0; i < m_Header.collisionTriangles.size; i++)
			collisionTrianglesArray.push_back(collisionTriangles[i]);

		collisonIndexBuffer = m_RenderDevice.GetObjectsFactory().CreateIndexBuffer(collisionTrianglesArray);
	}

	if (collisonVertexBuffer != nullptr && collisonIndexBuffer != nullptr)
	{
		auto collisionGeometry = GetRenderDevice().GetObjectsFactory().CreateGeometry();
		collisionGeometry->SetVertexBuffer(collisonVertexBuffer);
		collisionGeometry->SetIndexBuffer(collisonIndexBuffer);
		m_CollisionGeom = collisionGeometry;
	}
	else
	{
		m_CollisionGeom = nullptr;
	}
#endif

	m_IsAnimated = getSkeleton().isAnimBones() || getSkeleton().isBillboard() || getMaterials().IsAnimTextures() || getMiscellaneous().IsAnimated() || true;
	_ASSERT(m_Bytes.use_count() == 1);
	m_Bytes.reset();

	return true;
}

bool CM2::Delete()
{
	return false;
}
