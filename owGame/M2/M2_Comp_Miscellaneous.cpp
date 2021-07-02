#include "stdafx.h"

// Include
#include "M2.h"

// General
#include "M2_Comp_Miscellaneous.h"

CM2_Comp_Miscellaneous::CM2_Comp_Miscellaneous(const CM2& M2Object) 
	: m_HasMisc(false)
	, m_M2Object(M2Object)
{
}

CM2_Comp_Miscellaneous::~CM2_Comp_Miscellaneous()
{
}

void CM2_Comp_Miscellaneous::Load(const SM2_Header& M2Header, const std::shared_ptr<IFile>& File)
{
	// Attachments
	if (M2Header.attachments.size > 0)
	{
		const SM2_Attachment* m2Attachments = (const SM2_Attachment*)(File->getData() + M2Header.attachments.offset);
		for (uint32 i = 0; i < M2Header.attachments.size; i++)
			m_Attachments.push_back(SM2_Part_Attachment_Wrapper(m_M2Object, File, m2Attachments[i]));

		// Animated
		m_HasMisc = true;
	}

	// Attachments Lookup
	if (M2Header.attachmentLookup.size > 0)
	{
		const int16* m2AttachmentsLookup = (const int16*)(File->getData() + M2Header.attachmentLookup.offset);
		for (uint32 i = 0; i < M2Header.attachmentLookup.size; i++)
		{
			m_AttachmentsLookup.push_back(m2AttachmentsLookup[i]);
		}
	}

	// Events
	if (M2Header.events.size > 0)
	{
		const SM2_Event* m2Events = (const SM2_Event*)(File->getData() + M2Header.events.offset);
		for (uint32 i = 0; i < M2Header.events.size; i++)
			m_Events.push_back(SM2_Part_Event_Wrapper(m_M2Object, File, m2Events[i]));

		// Animated
		m_HasMisc = true;
	}

	// Lights
	if (M2Header.lights.size > 0)
	{
		const SM2_Light* m2Lights = (const SM2_Light*)(File->getData() + M2Header.lights.offset);
		for (uint32 i = 0; i < M2Header.lights.size; i++)
			m_Lights.push_back(SM2_Part_Light_Wrapper(m_M2Object, File, m2Lights[i]));

		// Animated
		m_HasMisc = true;
	}

	// Cameras
	if (M2Header.cameras.size > 0)
	{
		const SM2_Camera* m2Cameras = (const SM2_Camera*)(File->getData() + M2Header.cameras.offset);
		for (uint32 i = 0; i < M2Header.cameras.size; i++)
			m_Cameras.push_back(SM2_Part_Camera_Wrapper(m_M2Object, File, m2Cameras[i]));

		// Animated
		m_HasMisc = true;
	}

	// Cameras Lookup
	if (M2Header.camerasLookup.size > 0)
	{
		const int16* m2CamerasLookup = (const int16*)(File->getData() + M2Header.camerasLookup.offset);
		for (uint32 i = 0; i < M2Header.camerasLookup.size; i++)
			m_CamerasLookup.push_back(m2CamerasLookup[i]);
	}

#if 0
	// Ribbons
	if (M2Header.ribbon_emitters.size > 0)
	{
		const SM2_RibbonEmitter* m2Ribbons = (const SM2_RibbonEmitter*)(File->getData() + M2Header.ribbon_emitters.offset);
		for (uint32 i = 0; i < M2Header.ribbon_emitters.size; i++)
		{
			std::shared_ptr<CM2_RibbonEmitters> ribbon = std::make_shared<CM2_RibbonEmitters>(m_M2Object, File, m2Ribbons[i]);
			m_RibbonEmitters.push_back(ribbon);
		}

		// Animated
		m_HasMisc = true;
	}

#endif
	// Particle systems
	if (M2Header.particle_emitters.size > 0)
	{
		const SM2_Particle* m2ParticleSystems = (const SM2_Particle*)(File->getData() + M2Header.particle_emitters.offset);
		for (uint32 i = 0; i < M2Header.particle_emitters.size; i++)
		{
			particleSystems.push_back(std::make_shared<CM2_Part_ParticleSystem>(m_M2Object, File, m2ParticleSystems[i]));
		}

		// Animated
		m_HasMisc = true;
	}


}
