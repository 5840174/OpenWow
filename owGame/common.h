#pragma once

#include <sockets\\StdoutLog.h>
#include <sockets\\TcpSocket.h>
#include <sockets\\SocketThread.h>
#include <sockets\\SocketHandler.h>
#include <sockets\\SocketHandlerThread.h>


#define WOW_CLASSIC_1_12_1 1
#define WOW_BC_2_4_3       2  
#define WOW_WOTLK_3_3_5    3

#define WOW_CLIENT_VERSION WOW_WOTLK_3_3_5


/**
  * Enable or disable WMO models support
*/
#define USE_WMO_MODELS

/**
  * Enable or disable WMO culling by portals
*/
//#define USE_WMO_PORTALS_CULLING


/**
  * Enable or disable M2 models support
*/
#define USE_M2_MODELS


/**
  * Enable or disable M2 models support
*/
//#define USE_M2_PARTICLES

/**
  * Enable or disable M2 models support
*/
//#define USE_M2_RIBBONS





struct ISceneNodeProvider
{
	virtual void CreateInsances(const std::shared_ptr<ISceneNode>& Parent) const = 0;
};


// Interfaces
#include "Interfaces/ILiquid.h"
#include "Interfaces/SkyManager.h"
#include "Interfaces/Managers.h"

// Wow types
#include "WowTypes.h"
#include "WowConsts.h"
#include "WowTime.h"
#include "WowChunkUtils.h"
