#pragma once

// M2
#include "M2_Headers.h"

// Misc
#include "M2_Part_Attachment.h"
#include "M2_Part_Event.h"
#include "M2_Part_Light.h"
#include "M2_Part_Camera.h"
#include "M2_RibbonEmitters.h"
#include "M2_Part_ParticleSystem.h"

// FORWARD BEGIN
class CM2;
// FORWARD END

class ZN_API CM2_Comp_Miscellaneous
{
public:
	CM2_Comp_Miscellaneous(const CM2& M2Object);
	virtual ~CM2_Comp_Miscellaneous();

	void Load(const SM2_Header& M2Header, const std::shared_ptr<IFile>& File);

	bool IsAnimated() const { return m_HasMisc; }

public:
	bool                                                       isAttachmentExists(EM2_AttachmentPoint Index) const;
	std::shared_ptr<const CM2_Part_Attachment>                 getAttachment(EM2_AttachmentPoint Index) const;
	int16                                                      getAttachmentLookup(uint32 Index) const;
	std::shared_ptr<const CM2_Part_Attachment>                 getAttachmentDirect(int16 Index) const;

	std::shared_ptr<const CM2_Part_Event>                      getEventDirect(uint32 Index) const;
	std::shared_ptr<const CM2_Part_Light>                      getLightDirect(uint32 Index) const;
	const std::vector<std::shared_ptr<CM2_Part_Light>>&        GetLightsDirectArray() const;

	std::shared_ptr<const CM2_Part_Camera>                     getCamera(uint32 Index) const;
	int16                                                      getCameraLookup(uint32 Index) const;
	std::shared_ptr<const CM2_Part_Camera>                     getCameraDirect(int16 Index) const;

	const std::vector<std::shared_ptr<CM2_Part_ParticleSystem>>& GetParticles() const;

private:
	std::vector<std::shared_ptr<CM2_Part_Attachment>>          m_Attachments;
	std::vector<int16>                                         m_AttachmentsLookup;
	std::vector<std::shared_ptr<CM2_Part_Event>>               m_Events;
	std::vector<std::shared_ptr<CM2_Part_Light>>               m_Lights;
	std::vector<std::shared_ptr<CM2_Part_Camera>>              m_Cameras;
	std::vector<int16>                                         m_CamerasLookup;

	std::vector<std::shared_ptr<CM2_Part_RibbonEmitters>>      m_RibbonEmitters;
	std::vector<std::shared_ptr<CM2_Part_ParticleSystem>>      m_ParticleSystems;

	bool                                                       m_HasMisc;

private:
	const CM2& m_M2Object;
};
