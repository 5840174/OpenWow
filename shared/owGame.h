#pragma once

//---------------------------------------------------------//
//--                    Common                           --//
//---------------------------------------------------------//
#include "../owGame/common.h"

//---------------------------------------------------------//
//--               World of warcraft                     --//
//---------------------------------------------------------//

// DBC


// Liquid
#include "../owGame/Liquid/Liquid.h"
#include "../owGame/Liquid/LiquidBaseInstance.h"

// M2
#include "../owGame/M2/M2_Base_Instance.h"

// WMO
#include "../owGame/WMO/WMO_Base_Instance.h"
#include "../owGame/WMO/WMO_Doodad_Instance.h"
#include "../owGame/WMO/WMO_Liquid_Instance.h"

// Sky & Enviorement
#include "../owGame/Sky/SkyManager.h"

// Map 
#include "../owGame/Map/Map.h"

// World
#include "../owGame/World/WorldObjectsCreator.h"

// Renderer
#include "../owGame/Renderer/RendererWoW.h"

extern "C" __declspec(dllexport) IznPlugin* __stdcall GetPlugin(IBaseManager& BaseManager);