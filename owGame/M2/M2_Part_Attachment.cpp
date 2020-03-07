#include "stdafx.h"

// Include
#include "M2.h"

// General
#include "M2_Part_Attachment.h"

CM2_Part_Attachment::CM2_Part_Attachment(const CM2& M2Object, const std::shared_ptr<IFile>& File, const SM2_Attachment& M2Attachment)
	: m_M2Object(M2Object)
{
	m_Type = static_cast<M2_AttachmentType>(M2Attachment.id);
	m_BoneIndex = M2Attachment.bone;
	m_Position = M2Attachment.position;
	m_IsAnimateAttached.Initialize(M2Attachment.animate_attached, File);
}

CM2_Part_Attachment::~CM2_Part_Attachment()
{
}
