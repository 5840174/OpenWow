#pragma once

#include "M2.h"
#include "M2_Skin_Builder.h"

#define M2BUILDER_LOADBONES

class CM2_Builder
{
	friend CM2_Skin_Builder;
public:
	CM2_Builder(M2* _model);
	~CM2_Builder();

	bool Load();

	// Loader
	void Step1Header();
	void Step2GlobalLoops();
	void Step3Bones();
	void Step4Vertices();
	void Step5ColorAndTextures();
	void Step6Misc();
	void Step7Particles();
	void Step8Skins();
	void Step9Collision();

	// Parts loader

	void SetAnimated();
	bool IsAnimated() const { return m_ParentM2->m_IsAnimated; }

private:
	M2*						m_ParentM2;
	IFile*					m_F;
	SM2_Header				m_Header;

	// !! Don't delete this, it's temp data !! //

	// Global loop, animation and bones
	cGlobalLoopSeq			m_GlobalLoops;
	vector<IFile*>			animfiles;

	// Bones
	SM2_Bone*				m_M2Bones;

	// Vertices
	vector<SM2_Vertex>		m_Vertexes;

	// Skins
	SM2_SkinProfile*		m_Skins;

	// Textures
	SM2_Texture*			m_Textures;
	SM2_TextureWeight*		m_TexturesWeight;
	SM2_TextureTransform*	m_TexturesTransform;
};