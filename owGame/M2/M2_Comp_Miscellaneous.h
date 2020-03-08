#pragma once

// M2
#include "M2_Headers.h"

// Misc
#include "M2_Part_Attachment.h"
#include "M2_Part_Event.h"
#include "M2_Part_Light.h"
#include "M2_Part_Camera.h"
#include "M2_RibbonEmitters.h"
#include "Particle.h"
#include "ParticleSystem.h"

// FORWARD BEGIN
class CM2;
// FORWARD END

class CM2_Comp_Miscellaneous
{
public:
	CM2_Comp_Miscellaneous(const CM2& M2Object);
	virtual ~CM2_Comp_Miscellaneous();

	void Load(const SM2_Header& M2Header, const std::shared_ptr<IFile>& File);

	bool IsAnimated() const { return m_HasMisc; }

public:
	const SM2_Part_Attachment_Wrapper& getAttachmentDirect(uint32 _index) const
	{
		_ASSERT(_index < static_cast<uint32>(m_Attachments.size()));
		return (m_Attachments[_index]);
	}
	bool isAttachmentExists(M2_AttachmentType _index) const
	{
		if ((uint32)_index >= m_AttachmentsLookup.size())
		{
			return false;
		}
		int16 newIndex = m_AttachmentsLookup[(uint32)_index];
		return (newIndex != -1) && (newIndex < static_cast<int16>(m_Attachments.size()));
	}
	const SM2_Part_Attachment_Wrapper& getAttachment(M2_AttachmentType IndexIntoLookup) const
	{
		_ASSERT((uint32)IndexIntoLookup < m_AttachmentsLookup.size());
		int16 indexIntoDirect = m_AttachmentsLookup[(uint32)IndexIntoLookup];
		_ASSERT(indexIntoDirect != -1 && indexIntoDirect < static_cast<int16>(m_Attachments.size()));
		return (m_Attachments[indexIntoDirect]);
	}

	const SM2_Part_Event_Wrapper& getEventDirect(uint32 _index) const
	{
		_ASSERT(_index < m_Events.size());
		return (m_Events[_index]);
	}

	const SM2_Part_Light_Wrapper& getLightDirect(uint32 _index) const
	{
		_ASSERT(_index < m_Lights.size());
		return (m_Lights[_index]);
	}

	const SM2_Part_Camera_Wrapper& getCameraDirect(uint32 _index) const
	{
		_ASSERT(_index < static_cast<uint32>(m_Cameras.size()));
		return (m_Cameras[_index]);
	}

	const SM2_Part_Camera_Wrapper& getCamera(uint32 IndexIntoLookup) const
	{
		_ASSERT(IndexIntoLookup < m_CamerasLookup.size());
		int16 indexIntoDirect = m_CamerasLookup[IndexIntoLookup];
		_ASSERT(indexIntoDirect != -1 && indexIntoDirect < static_cast<int16>(m_Cameras.size()));
		return (m_Cameras[indexIntoDirect]);
	}

private:
	// Attachments, events, lights and cameras
	std::vector<SM2_Part_Attachment_Wrapper>  m_Attachments;
	std::vector<int16>                        m_AttachmentsLookup;
	std::vector<SM2_Part_Event_Wrapper>       m_Events;
	std::vector<SM2_Part_Light_Wrapper>       m_Lights;
	std::vector<SM2_Part_Camera_Wrapper>      m_Cameras;
	std::vector<int16>                        m_CamerasLookup;

	// Particles
	std::vector<std::shared_ptr<CM2_RibbonEmitters>> m_RibbonEmitters;
	std::vector<std::shared_ptr<CM2_ParticleSystem>> particleSystems;

	bool                                 m_HasMisc;

private:
	const CM2& m_M2Object;
};