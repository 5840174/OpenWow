#pragma once

#include "MDX_Headers.h"

#include "MDX_Part_Bone.h"
#include "MDX_Part_Color.h"
#include "MDX_Part_TextureAnim.h"
#include "MDX_Part_TextureWeight.h"

#include "MDX_Part_Light.h"
#include "MDX_Part_Camera.h"

#include "Particle.h"
#include "ParticleSystem.h"
#include "RibbonEmitter.h"

#include "MDX_Skin.h"

enum BlendModes
{
	M2BLEND_OPAQUE,
	M2BLEND_ALPHA_KEY,
	M2BLEND_ALPHA,
	M2BLEND_NO_ALPHA_ADD,
	M2BLEND_ADD,
	M2BLEND_MOD,
	M2BLEND_MOD2X
};

class MDX : public CRefItem
{
public:
	MDX(cstring name);
	~MDX();

	void Init(bool forceAnim = false);
	bool IsLoaded() { return m_Loaded; }

	void Render();
	void updateEmitters(float dt);

	ModelHeader header;

    R_Buffer* __vb;

private:
	string m_FileName;
	bool m_Loaded;
	string m_ModelInternalName;

private:
	std::vector<Model_Skin*> m_Skins;

public:
	bool animated;
	bool animGeometry, animTextures, animBones;

	bool forceAnim;

	M2Sequence* m_Sequences;
	uint32* m_GlobalLoops;

	MDX_Part_Bone* m_Part_Bones;

	MDX_Part_Color* m_Colors;
	M2Texture* m_M2Textures;
	MDX_Part_TextureWeight* m_TextureWeights;
	MDX_Part_TextureAnim* m_TexturesAnims;

	MDX_Part_Light* m_Lights;
	MDX_Part_Camera* m_Cameras;

#ifdef MDX_PARTICLES_ENABLE
	ParticleSystem* particleSystems;
	RibbonEmitter* ribbons;
#endif

	void drawModel();
	void initCommon(IFile* f);
	bool isAnimated(IFile* f);
	void initAnimated(IFile* f);

	void animate(uint32 _animationIndex);
	void calcBones(uint32 _animationIndex, int time);

	void lightsOn(uint32 lbase);
	void lightsOff(uint32 lbase);

public:
	std::vector<SmartTexturePtr> m_Textures;
	int m_SpecialTextures[TEXTURE_MAX];
	R_Texture* m_TextureReplaced[TEXTURE_MAX];
	bool m_TexturesUseSpecialTexture[TEXTURE_MAX];

	bool m_IsBillboard;

	BoundingBox m_Bounds;
	bool animcalc;
	int m_AnimationIndex, m_AnimationTime;

	friend class MDX_Skin_Batch;
};


struct MDXDeleter
{
	void operator()(MDX* p)
	{
		GetManager<IMDXManager>()->Delete(p);
	}
};
typedef SmartPtr<MDX, MDXDeleter> SmartMDXPtr;
