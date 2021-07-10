#include "stdafx.h"

// General
#include "DayNightPhase.h"

SDayNightPhase::SDayNightPhase()
{}

SDayNightPhase::SDayNightPhase(std::shared_ptr<IFile> f)
{
    float h, m;

    f->seekRelative(4); // Always 0x46
    f->readBytes(&h, 4);
    f->seekRelative(4);
    f->readBytes(&m, 4);

    f->seekRelative(4);
    f->readBytes(&dayIntensity, 4);
    f->seekRelative(4);
    f->readBytes(&dayColor.r, 4);
    f->seekRelative(4);
    f->readBytes(&dayColor.g, 4);
    f->seekRelative(4);
    f->readBytes(&dayColor.b, 4);
    f->seekRelative(4);
    f->readBytes(&dayDir.x, 4);
    f->seekRelative(4);
    f->readBytes(&dayDir.y, 4);
    f->seekRelative(4);
    f->readBytes(&dayDir.z, 4);
	dayDir = Fix_XZmY(dayDir);
    
    //
    f->seekRelative(4);
    f->readBytes(&nightIntensity, 4);
    f->seekRelative(4);
    f->readBytes(&nightColor.r, 4);
    f->seekRelative(4);
    f->readBytes(&nightColor.g, 4);
    f->seekRelative(4);
    f->readBytes(&nightColor.b, 4);
    f->seekRelative(4);
    f->readBytes(&nightDir.x, 4);
    f->seekRelative(4);
    f->readBytes(&nightDir.y, 4);
    f->seekRelative(4);
    f->readBytes(&nightDir.z, 4);
	nightDir = Fix_XZmY(nightDir);

    //
    f->seekRelative(4);
    f->readBytes(&ambientIntensity, 4);
    f->seekRelative(4);
    f->readBytes(&ambientColor.r, 4);
    f->seekRelative(4);
    f->readBytes(&ambientColor.g, 4);
    f->seekRelative(4);
    f->readBytes(&ambientColor.g, 4);

    //
    f->seekRelative(4);
    f->readBytes(&fogDepth, 4);
    f->seekRelative(4);
    f->readBytes(&fogIntensity, 4);
    f->seekRelative(4);
    f->readBytes(&fogColor.r, 4);
    f->seekRelative(4);
    f->readBytes(&fogColor.g, 4);
    f->seekRelative(4);
    f->readBytes(&fogColor.b, 4);

    uint32 time = (uint32)h * 60 * 2 + (uint32)m * 2;
}

SDayNightPhase::SDayNightPhase(const SDayNightPhase& a, const SDayNightPhase& b, float r)
{
    float ir = 1.0f - r;

    // Day
    dayIntensity = a.dayIntensity * ir + b.dayIntensity * r;
    dayColor = a.dayColor * ir + b.dayColor * r;
    dayDir = a.dayDir * ir + b.dayDir * r;

    // Night
    nightIntensity = a.nightIntensity * ir + b.nightIntensity * r;
    nightColor = a.nightColor * ir + b.nightColor * r;
    nightDir = a.nightDir * ir + b.nightDir * r;

    // Ambient
    ambientIntensity = a.ambientIntensity * ir + b.ambientIntensity * r;
    ambientColor = a.ambientColor * ir + b.ambientColor * r;

    // Fog
    fogDepth = a.fogDepth * ir + b.fogDepth * r;
    fogIntensity = a.fogIntensity * ir + b.fogIntensity * r;
    fogColor = a.fogColor * ir + b.fogColor * r;
}

//

/*void DayNightPhase::setupLighting()
{
    // Setup day lighting
    if (dayIntensity > 0)
    {
        m_dirLightDay.ambient = ambientColor * ambientIntensity * dayIntensity;
        m_dirLightDay.diffuse = dayColor * dayIntensity;
        m_dirLightDay.Direction = dayDir;
    }

    // Setup night lighting
    if (nightIntensity > 0)
    {
        m_dirLightNight.ambient = ambientColor * ambientIntensity * nightIntensity;
        m_dirLightNight.diffuse = nightColor * nightIntensity;
        m_dirLightNight.Direction = nightDir;
    }
}*/
