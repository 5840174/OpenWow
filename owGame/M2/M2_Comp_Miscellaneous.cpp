#include "stdafx.h"

// Include
#include "M2.h"

// General
#include "M2_Comp_Miscellaneous.h"

CM2_Comp_Miscellaneous::CM2_Comp_Miscellaneous(const M2& M2Object) 
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
		SM2_Attachment* m2Attachments = (SM2_Attachment*)(File->getData() + M2Header.attachments.offset);
		for (uint32 i = 0; i < M2Header.attachments.size; i++)
		{
			std::shared_ptr<CM2_Part_Attachment> attachment = std::make_shared<CM2_Part_Attachment>(m_M2Object, File, m2Attachments[i]);
			m_Attachments.push_back(attachment);
		}

		// Animated
		m_HasMisc = true;
	}

	// Attachments Lookup
	if (M2Header.attachmentLookup.size > 0)
	{
		int16* m2AttachmentsLookup = (int16*)(File->getData() + M2Header.attachmentLookup.offset);
		for (uint32 i = 0; i < M2Header.attachmentLookup.size; i++)
		{
			m_AttachmentsLookup.push_back(m2AttachmentsLookup[i]);
		}
	}

	// Events
	if (M2Header.events.size > 0)
	{
		SM2_Event* m2Events = (SM2_Event*)(File->getData() + M2Header.events.offset);
		for (uint32 i = 0; i < M2Header.events.size; i++)
		{
			std::shared_ptr<CM2_Part_Event> event = std::make_shared<CM2_Part_Event>(m_M2Object, File, m2Events[i]);
			m_Events.push_back(event);
		}

		// Animated
		m_HasMisc = true;
	}

	// Lights
	if (M2Header.lights.size > 0)
	{
		SM2_Light* m2Lights = (SM2_Light*)(File->getData() + M2Header.lights.offset);
		for (uint32 i = 0; i < M2Header.lights.size; i++)
		{
			std::shared_ptr<CM2_Part_Light> light = std::make_shared<CM2_Part_Light>(m_M2Object, File, m2Lights[i]);
			m_Lights.push_back(light);
		}

		// Animated
		m_HasMisc = true;
	}

	// Cameras
	if (M2Header.cameras.size > 0)
	{
		SM2_Camera* m2Cameras = (SM2_Camera*)(File->getData() + M2Header.cameras.offset);
		for (uint32 i = 0; i < M2Header.cameras.size; i++)
		{
			std::shared_ptr<CM2_Part_Camera> camera = std::make_shared<CM2_Part_Camera>(m_M2Object, File, m2Cameras[i]);
			m_Cameras.push_back(camera);
		}

		// Animated
		m_HasMisc = true;
	}

	// Cameras Lookup
	if (M2Header.camerasLookup.size > 0)
	{
		int16* m2CamerasLookup = (int16*)(File->getData() + M2Header.camerasLookup.offset);
		for (uint32 i = 0; i < M2Header.camerasLookup.size; i++)
		{
			m_CamerasLookup.push_back(m2CamerasLookup[i]);
		}
	}

	// Ribbons
	if (M2Header.ribbon_emitters.size > 0)
	{
		SM2_RibbonEmitter* m2Ribbons = (SM2_RibbonEmitter*)(File->getData() + M2Header.ribbon_emitters.offset);
		for (uint32 i = 0; i < M2Header.ribbon_emitters.size; i++)
		{
			std::shared_ptr<CM2_RibbonEmitters> ribbon = std::make_shared<CM2_RibbonEmitters>(m_M2Object, File, m2Ribbons[i]);
			m_RibbonEmitters.push_back(ribbon);
		}

		// Animated
		m_HasMisc = true;
	}

	// Particle systems
	if (M2Header.particle_emitters.size > 0)
	{
		/*SM2_Particle* Particles = (SM2_Particle*)(File->getData() + M2Header.particle_emitters.offset);
		for (uint32 i = 0; i < M2Header.particle_emitters.size; i++)
		{
			CM2_ParticleSystem* particle = new CM2_ParticleSystem(m_M2, File, Particles[i], m_GlobalLoops);
			miscellaneous->particleSystems.push_back(particle);
		}

		// Animated
		miscellaneous->m_HasMisc = true;*/
	}
}

void CM2_Comp_Miscellaneous::update(double _time, double _dTime)
{
	for (const auto& it : particleSystems)
	{
		it->update(_time, _dTime);
	}
}

void CM2_Comp_Miscellaneous::calc(uint16 anim, uint32 time, uint32 globalTime, cmat4 _worldMat)
{
	/*for (const auto& it : m_Lights)
	{
		it->setup(anim, time, globalTime);
	}

	for (const auto& it : m_Cameras)
	{
		it->calc(time, globalTime);
	}

	for (const auto& it : m_RibbonEmitters)
	{
		it->setup(anim, time, globalTime, _worldMat);
	}

	for (const auto& it : particleSystems)
	{
		it->setup(anim, time, globalTime);
	}*/
}

void CM2_Comp_Miscellaneous::render(cmat4 _worldMat)
{

	for (const auto& it : m_Attachments)
	{
		it->render(_worldMat);
	}

	for (const auto& it : m_RibbonEmitters)
	{
		it->Render(_worldMat);
	}

	for (const auto& it : particleSystems)
	{
		it->Render3D(_worldMat);
	}
}
