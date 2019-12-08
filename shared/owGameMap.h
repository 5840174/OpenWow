#pragma once

//---------------------------------------------------------//
//--                    Common                           --//
//---------------------------------------------------------//
#include "../owGameMap/common.h"

//---------------------------------------------------------//
//--               World of warcraft                     --//
//---------------------------------------------------------//

// Map
#include "../owGameMap/Map.h"

// Environment
#include "../owGameMap/DayNightCycle.h"
#include "../owGameMap/SkyManager.h"
#include "../owGameMap/EnvironmentManager.h"

// WMO
#include "../owGameMap/WMO/WMO_Base_Instance.h"
#include "../owGameMap/WMO/WMO_Doodad_Instance.h"
#include "../owGameMap/WMO/WMO_Liquid_Instance.h"
#include "../owGameMap/WMO/WMOsManager.h"

// Render stuff
#include "../owGameMap/CreatePasses.h"

extern "C" __declspec(dllexport) IznPlugin* WINAPI GetPlugin(IBaseManager* BaseManager);
