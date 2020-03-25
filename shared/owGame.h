#pragma once

//---------------------------------------------------------//
//--                    Common                           --//
//---------------------------------------------------------//
#include "../owGame/common.h"

//---------------------------------------------------------//
//--               World of warcraft                     --//
//---------------------------------------------------------//

#include "../owGame/WoWChunkReader.h"

// DBC
#include "../owGame/DBC/DBC__Storage.h"

// Client
#include "../owGame/Client/ObjectGUID.h"
#include "../owGame/Client/Client.h"

// Liquid
#include "../owGame/Liquid/Liquid.h"
#include "../owGame/Liquid/LiquidInstance.h"
#include "../owGame/Liquid/RenderPass_Liquid.h"

// M2
#include "../owGame/M2/M2_Base_Instance.h"
#include "../owGame/M2/RenderPass_M2.h"
#include "../owGame/M2/RenderPass_M2Instanced.h"

// WMO
#include "../owGame/WMO/WMO_Base_Instance.h"
#include "../owGame/WMO/WMO_Doodad_Instance.h"
#include "../owGame/WMO/WMO_Liquid_Instance.h"
#include "../owGame/WMO/RenderPass_WMO.h"

// Sky & Enviorement
#include "../owGame/Sky/SkyManager.h"
#include "../owGame/Sky/Environment/EnvironmentManager.h"
#include "../owGame/Sky/RenderPass_Sky.h"

// Map 
#include "../owGame/Map/Map.h"
#include "../owGame/Map/RenderPass_MapTile.h"
#include "../owGame/Map/RenderPass_WDL.h"
#include "../owGame/Map/RenderPass_MapChunk.h"

// World
#include "../owGame/World/WorldObjectsCreator.h"

