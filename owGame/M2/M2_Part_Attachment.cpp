#include "stdafx.h"

// Include
#include "M2.h"

// General
#include "M2_Part_Attachment.h"

CM2_Part_Attachment::CM2_Part_Attachment(const CM2& M2Object, const std::shared_ptr<IByteBuffer>& ByteBuffer, const SM2_Attachment& M2Attachment)
	: m_M2Object(M2Object)
{
	if (M2Attachment.id >= static_cast<uint32>(EM2_AttachmentPoint::Count))
		throw CException("CM2_Part_Attachment::CM2_Part_Attachment: AttachmentID not is 'EM2_AttachmentPoint' bounds.");

	m_Type = static_cast<EM2_AttachmentPoint>(M2Attachment.id);
	m_BoneIndex = M2Attachment.bone;
	_ASSERT(m_BoneIndex != UINT16_MAX);

	m_Position = Fix_From_XZmY_To_XYZ(M2Attachment.position);
	m_IsAnimateAttached.Initialize(M2Attachment.animate_attached, ByteBuffer, M2Object.getSkeleton().GetAnimFiles());
}

CM2_Part_Attachment::~CM2_Part_Attachment()
{
}
