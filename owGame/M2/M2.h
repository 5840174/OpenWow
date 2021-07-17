#pragma once

#include "M2_Headers.h"

// Components
#include "M2_Comp_Materials.h"
#include "M2_Comp_Miscellaneous.h"
#include "M2_Comp_Skeleton.h"

// Skins
#include "M2_Skin.h"

class ZN_API CM2 
	: public ISceneNodeProvider
	, public CLoadableObject
{
public:
	CM2(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const std::string& FileName);
	virtual ~CM2();


	// CM2
	std::string                                     getFilename() const { return m_FileName; }
	std::string                                     getFilenameWithoutExt() const { return m_FileNameWithoutExt; }
	std::string                                     getFilePath() const { return m_FilePath; }

	const SM2_Header&                               GetHeader() const { return m_Header; }
	std::string                                     getUniqueName() const { return m_UniqueName; }
	const BoundingBox&                              GetBounds() const { return m_Bounds; }

	const CM2_Comp_Materials&                       getMaterials() const { return *m_Materials; }
	const CM2_Comp_Miscellaneous&                   getMiscellaneous() const { return *m_Miscellaneous; }
	const CM2_Comp_Skeleton&                        getSkeleton() const { return *m_Skeleton; }

	const bool                                      isAnimated() const { return m_IsAnimated; }

	const IBaseManager&                             GetBaseManager() const { return m_BaseManager; }
	IRenderDevice&                                  GetRenderDevice() const { return m_RenderDevice; }


	// ISceneNodeProvider
	void                                            CreateInsances(const std::shared_ptr<ISceneNode>& Parent) const override;


	// ILoadable
	bool                                            Load() override;
	bool                                            Delete() override;


private:
	std::string							            m_FileName;
	std::string							            m_FileNameWithoutExt;
	std::string							            m_FilePath;
	std::shared_ptr<IByteBuffer>                    m_Bytes;

	SM2_Header				                        m_Header;
	std::string							            m_UniqueName;
	BoundingBox							            m_Bounds;

	std::unique_ptr<CM2_Comp_Materials>             m_Materials;
	std::unique_ptr<CM2_Comp_Miscellaneous>         m_Miscellaneous;
	std::unique_ptr<CM2_Comp_Skeleton>              m_Skeleton;

	bool                                            m_IsAnimated;

	std::vector<std::shared_ptr<CM2_Skin>>          m_Skins;

private:
	// Buffers and geom
	std::shared_ptr<IModel>				            m_CollisionGeom;
	uint32								            m_CollisionIndCnt;
	uint32								            m_CollisionVetCnt;

private:
	IBaseManager&                                   m_BaseManager;
	IRenderDevice&                                  m_RenderDevice;
};
