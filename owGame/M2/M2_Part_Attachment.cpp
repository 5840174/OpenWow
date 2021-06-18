#include "stdafx.h"

// Include
#include "M2.h"

// General
#include "M2_Part_Attachment.h"

SM2_Part_Attachment_Wrapper::SM2_Part_Attachment_Wrapper(const CM2& M2Object, const std::shared_ptr<IFile>& File, const SM2_Attachment& M2Attachment)
	: m_M2Object(M2Object)
{
	_ASSERT(M2Attachment.id < static_cast<uint32>(M2_AttachmentType::Count));
	m_Type = static_cast<M2_AttachmentType>(M2Attachment.id);
	m_BoneIndex = M2Attachment.bone;
	_ASSERT(m_BoneIndex != UINT16_MAX);
	m_Position = Fix_XZmY(M2Attachment.position);
	m_IsAnimateAttached.Initialize(M2Attachment.animate_attached, File, M2Object.getSkeleton().GetAnimFiles());
}

SM2_Part_Attachment_Wrapper::~SM2_Part_Attachment_Wrapper()
{
}
